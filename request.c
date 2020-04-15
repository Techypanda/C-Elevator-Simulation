#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "request.h"
#include "lifts.h"

request* createRequest(int inRequest, int inDestination) {
    request* newRequest = malloc(sizeof(request));
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
                printf("Line %d is invalid, program will now stop.\n", linecount);
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
