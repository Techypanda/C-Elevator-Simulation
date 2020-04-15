#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "queue.h"
#include "lifts.h"
#include "request.h"

void* lift(void* args) {
    void* currentRequest;
    request* tempRequest;
    int localFinishRead, localCount, sleepNeeded, distance;
    #ifndef NOTSLEEP
    int localSleepTimer;
    #endif
    liftStruct* thisLift = (liftStruct*)args;
    pthread_mutex_lock(thisLift->mutexLock);
    localFinishRead = *(thisLift->finishedRead);
    localCount = thisLift->buffer->list->size;
    #ifndef NOTSLEEP
    localSleepTimer = thisLift->liftTimer;
    #endif
    pthread_mutex_unlock(thisLift->mutexLock);
    sleepNeeded = FALSE;
    while (localFinishRead != TRUE || localCount != 0) {
        pthread_mutex_lock(thisLift->mutexLock);
        if (thisLift->buffer->list->size < 1 && *(thisLift->finishedRead) != TRUE) {
            #ifdef DEBUG
            printf("THREAD %ld (AKA: LIFT %d): Signaling condition empty in lifts.c (localFinishedRead = %d)\n", pthread_self(), thisLift->liftNumber, localFinishRead);
            #endif
            pthread_cond_signal(thisLift->empty);
        } else {
            currentRequest = dequeue(thisLift->buffer);
            if (currentRequest != NULL) {
                if (thisLift->previousRequest == NULL) {
                    distance = abs(1 - ((request*)currentRequest)->requestFloor) + abs(((request*)currentRequest)->destinationFloor - ((request*)currentRequest)->requestFloor);
                    *((thisLift->liftReturnVals) + 0) += distance;
                    *((thisLift->liftReturnVals) + 1) += 1;
                    fprintf(thisLift->out_sim_file,
                        "Lift-%d Operation\nPrevious position: Floor 1 (I haven't had any requests yet.)\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                        thisLift->liftNumber, ((request*)currentRequest)->requestFloor, ((request*)currentRequest)->destinationFloor, 1, ((request*)currentRequest)->requestFloor, ((request*)currentRequest)->requestFloor, ((request*)currentRequest)->destinationFloor,
                        distance, *((thisLift->liftReturnVals) + 1), *((thisLift->liftReturnVals) + 0), ((request*)currentRequest)->requestFloor
                    );
                    thisLift->previousRequest = ((request*)currentRequest);
                    #ifndef NOTSLEEP
                    sleepNeeded = TRUE;
                    #endif
                } else {
                    distance = abs(thisLift->previousRequest->destinationFloor - ((request*)currentRequest)->requestFloor) + abs(((request*)currentRequest)->destinationFloor - ((request*)currentRequest)->requestFloor);
                    *((thisLift->liftReturnVals) + 0) += distance;
                    *((thisLift->liftReturnVals) + 1) += 1;
                    fprintf(thisLift->out_sim_file,
                        "Lift-%d Operation\nPrevious position: Floor %d\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                        thisLift->liftNumber, thisLift->previousRequest->destinationFloor, ((request*)currentRequest)->requestFloor, ((request*)currentRequest)->destinationFloor, thisLift->previousRequest->destinationFloor, ((request*)currentRequest)->requestFloor,
                        ((request*)currentRequest)->requestFloor, ((request*)currentRequest)->destinationFloor, distance, *((thisLift->liftReturnVals) + 1), *((thisLift->liftReturnVals) + 0), ((request*)currentRequest)->requestFloor
                    );
                    tempRequest = thisLift->previousRequest;
                    thisLift->previousRequest = ((request*)currentRequest);
                    free(tempRequest);
                    #ifndef NOTSLEEP
                    sleepNeeded = TRUE;
                    #endif
                }
            } else {
                #ifdef DEBUG
                printf("\n!!!! RECIEVED A NULL ON THREAD %ld (AKA: LIFT %d)!!!!\n", pthread_self(), thisLift->liftNumber);
                #endif
                /* This occurs once at the end and doesnt happen again as it hasnt updated localFinishRead. */
            }
        }
        localFinishRead = *(thisLift->finishedRead);
        localCount = thisLift->buffer->list->size;
        pthread_mutex_unlock(thisLift->mutexLock);
        if (sleepNeeded == TRUE) {
            #ifndef NOTSLEEP
            sleepNeeded = FALSE;
            sleep(localSleepTimer);
            #endif
        }
    }
    free(thisLift->previousRequest);
    return NULL;
}

liftStruct* initLiftStruct(queue* inBuffer, int inTimer, int whatLift,
pthread_mutex_t* inLock, pthread_cond_t* inFullCond, pthread_cond_t* inEmptyCond,
int* inFinishedRead, int inMaxBufferSize, FILE* inFile) {
    liftStruct* newLiftStruct = (liftStruct*)malloc(sizeof(liftStruct));
    newLiftStruct->buffer = inBuffer;
    newLiftStruct->previousRequest = NULL;
    newLiftStruct->finishedRead = inFinishedRead;
    newLiftStruct->liftTimer = inTimer;
    newLiftStruct->liftNumber = whatLift;
    newLiftStruct->mutexLock = inLock;
    newLiftStruct->full = inFullCond;
    newLiftStruct->empty = inEmptyCond;
    newLiftStruct->maxBufferSize = inMaxBufferSize;
    newLiftStruct->out_sim_file = inFile;
    newLiftStruct->liftReturnVals = (int*)malloc(sizeof(int) * 2);
    *((newLiftStruct->liftReturnVals) + 0) = 0;
    *((newLiftStruct->liftReturnVals) + 1) = 0;
    /* Hopefully this file is verified cause otherwise big f**** rip */
    return newLiftStruct;
}

void freeLiftStruct(liftStruct* toFree) {
    free(toFree->liftReturnVals);
    free(toFree);
}
