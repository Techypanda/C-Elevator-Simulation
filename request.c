#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include "request.h"
#include "lifts.h"
/*******************************************************************************
*   PURPOSE: This C File contains definitions to do with requests aswell as the
*   implmentation of the request elevators.
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/

/*******************************************************************************
*   PURPOSE: creates a request on the heap.
*   IMPORTS: int inRequest, int inDestination
*   EXPORTS: request* outRequest
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
request* createRequest(int inRequest, int inDestination) {
    request* newRequest = malloc(sizeof(request));
    newRequest->requestFloor = inRequest;
    newRequest->destinationFloor = inDestination;
    return newRequest;
}
/*******************************************************************************
*   PURPOSE: creates a request on the stack.
*   IMPORTS: int inRequest, int inDestination
*   EXPORTS: request outRequest
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
request createStackRequest(int inRequest, int inDestination) {
    request newRequest;
    newRequest.requestFloor = inRequest;
    newRequest.destinationFloor = inDestination;
    return newRequest;
}
/*******************************************************************************
*   PURPOSE: creates a request on the shared memory.
*   IMPORTS: int inRequest, int inDestination
*   EXPORTS: request* outRequest
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
request* createSharedRequest(int inRequest, int inDestination) {
    request* newRequest = (request*)mmap(NULL,sizeof(request),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    newRequest->requestFloor = inRequest;
    newRequest->destinationFloor = inDestination;
    return newRequest;
}
/*******************************************************************************
*   PURPOSE: Lift 1-3 PTHREAD implementation.
*   IMPORTS: void* args
*   EXPORTS: NULL
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void* requestt(void* args) {
    int linecount;
    FILE* file;
    liftStruct* fakeLift;
    int destination, from, fscanfReturn;
    file = fopen("sim_input", "r");
    linecount = 1;
    fakeLift = (liftStruct*)args; /* This is not a actual lift */
    if (file == NULL) {
        fprintf(stderr, "Unable to read sim_input, Did you put a file called sim_input in my directory?\nTerminating!\n");
        pthread_mutex_lock(fakeLift->mutexLock);
        *(fakeLift->finishedRead) = TRUE;
        pthread_mutex_unlock(fakeLift->mutexLock);
        exit(EXIT_FAILURE);
    }
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while (fscanfReturn != EOF) {
        pthread_mutex_lock(fakeLift->mutexLock);
        #ifdef DEBUG
        printf("LIFT ZERO WAITING ON EMPTY\n");
        #endif
        pthread_cond_wait(fakeLift->empty, fakeLift->mutexLock);
        while (fscanfReturn != EOF && (fakeLift->buffer->list->size != fakeLift->maxBufferSize)) {
            if (fscanfReturn != 2) {
                printf("A Line contains invalid amount of values, program will now stop.\n");
                *(fakeLift->finishedRead) = TRUE;
                fclose(file);
                pthread_mutex_unlock(fakeLift->mutexLock);
                return NULL;
            } else {
                if (from < 1 || from > 20) {
                    fprintf(stderr, "Line %d contained a number greater than 20 or less than 1 for the request floor, ignoring line.\n", linecount);
                    linecount += 1;
                    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                } else if (destination < 1 || destination > 20) {
                    fprintf(stderr, "Line %d contained a number greater than 20 or less than 1 for the destination floor, ignoring line.\n", linecount);
                    linecount += 1;
                    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                } else {
                    enqueue(createRequest(from, destination), fakeLift->buffer);
                    fprintf(fakeLift->out_sim_file,"--------------------------------------------\nNew Lift Request From Floor %d to Floor %d\nRequest No: %d\n--------------------------------------------\n\n",
                    from, destination, linecount);
                    fflush(fakeLift->out_sim_file);
                    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                    linecount += 1;
                }
            }
        }
        pthread_mutex_unlock(fakeLift->mutexLock);
    }
    pthread_mutex_lock(fakeLift->mutexLock);
    *(fakeLift->finishedRead) = TRUE;
    pthread_mutex_unlock(fakeLift->mutexLock);
    fclose(file);
    return NULL;
}

/*******************************************************************************
*   PURPOSE: Lift 1-3 Process Implementation.
*   IMPORTS: void* args
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void processRequest(void* args) {
    #ifdef DEBUG
    int tester;
    #endif
    processLift** liftZero;
    FILE* file;
    int destination, from, fscanfReturn, linecount;
    liftZero = (processLift**)args;
    file = fopen("sim_input", "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to read sim_input, Did you put a file called sim_input in my directory?\nTerminating!\n");
        **((*liftZero)->finishedRead) = TRUE;
        exit(EXIT_FAILURE);
    }
    linecount = 1;
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while (fscanfReturn != EOF)
    {
        sem_wait(*((*liftZero)->semaphoreEmpty));
        sem_wait(*((*liftZero)->mutexSem));
        if (fscanfReturn != 2) {
            printf("A Line contains too many values or an invalid character, program will now stop.\n");
            **((*liftZero)->finishedRead) = TRUE;
            fclose(file);
            exit(EXIT_FAILURE);
        } else {
            if (from < 1 || from > 20) {
                fprintf(stderr, "Line %d contained a number greater than 20 or less than 1 for the request floor, ignoring line.\n", linecount);
                linecount += 1;
                fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                sem_post(*((*liftZero)->semaphoreEmpty));
            } else if (destination < 1 || destination > 20) {
                fprintf(stderr, "Line %d contained a number greater than 20 or less than 1 for the destination floor, ignoring line.\n", linecount);
                linecount += 1;
                fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                sem_post(*((*liftZero)->semaphoreEmpty));
            } else {
                arrayEnqueue(createStackRequest(from, destination), (*liftZero)->buffer);
                fprintf((**((*liftZero)->out_sim_file)),"--------------------------------------------\nNew Lift Request From Floor %d to Floor %d\nRequest No: %d\n--------------------------------------------\n\n",
                from, destination, linecount);
                fflush(**((*liftZero)->out_sim_file));
                fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                linecount += 1;
                sem_post(*((*liftZero)->semaphoreFull));
            }
        }
        sem_post(*((*liftZero)->mutexSem));
    }
    **((*liftZero)->finishedRead) = TRUE;
    fclose(file);
}
