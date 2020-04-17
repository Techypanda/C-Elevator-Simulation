#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include "request.h"
#include "lifts.h"

request* createRequest(int inRequest, int inDestination) {
    request* newRequest = malloc(sizeof(request));
    newRequest->requestFloor = inRequest;
    newRequest->destinationFloor = inDestination;
    return newRequest;
}

request createStackRequest(int inRequest, int inDestination) {
    request newRequest;
    newRequest.requestFloor = inRequest;
    newRequest.destinationFloor = inDestination;
    return newRequest;
}

request* createSharedRequest(int inRequest, int inDestination) {
    request* newRequest = (request*)mmap(NULL,sizeof(request),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    newRequest->requestFloor = inRequest;
    newRequest->destinationFloor = inDestination;
    return newRequest;
}

void* requestt(void* args) {
    int linecount;
    FILE* file;
    liftStruct* fakeLift;
    int destination, from, fscanfReturn;
    file = fopen("sim_input", "r");
    linecount = 1;
    fakeLift = (liftStruct*)args; /* This is not a actual lift */
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

/*
sem_getvalue( *((*liftZero)->semaphoreFull), &tester);
printf("SEM VALUE: %d\n", tester);
int tester;
*/
void processRequest(void* args) {
    #ifdef DEBUG
    int tester;
    #endif
    processLift** liftZero;
    FILE* file;
    int destination, from, fscanfReturn, linecount;
    liftZero = (processLift**)args;
    file = fopen("sim_input", "r");
    linecount = 1;
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while (fscanfReturn != EOF)
    {
        #ifdef DEBUG
        sem_getvalue( *((*liftZero)->semaphoreEmpty), &tester);
        printf("EMPTY SEM VALUE BEFORE: %d\n", tester);
        #endif
        sem_wait(*((*liftZero)->semaphoreEmpty));
        #ifdef DEBUG
        sem_getvalue( *((*liftZero)->semaphoreEmpty), &tester);
        printf("SEM VALUE AFTER: %d\n", tester);
        #endif
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
                sem_wait(*((*liftZero)->liftZeroFileSem));
                fprintf((**((*liftZero)->out_sim_file)),"--------------------------------------------\nNew Lift Request From Floor %d to Floor %d\nRequest No: %d\n--------------------------------------------\n\n",
                from, destination, linecount);
                fflush(**((*liftZero)->out_sim_file));
                fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
                linecount += 1;
                #ifdef DEBUG
                sem_getvalue( *((*liftZero)->semaphoreFull), &tester);
                printf("FULL SEM VALUE BEFORE: %d\n", tester);
                #endif
                sem_post(*((*liftZero)->semaphoreFull));
                #ifdef DEBUG
                sem_getvalue( *((*liftZero)->semaphoreFull), &tester);
                printf("FULL SEM VALUE AFTER: %d\n", tester);
                #endif /* VERIFICATION NEEDED */
                sem_post(*((*liftZero)->requestFileSem));
            }
        }
    }
    **((*liftZero)->finishedRead) = TRUE;
    fclose(file);
}
