#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "queue.h"
#include "lifts.h"
#include "request.h"

void* lift(void* args) {
    void* currentRequest;
    int localFinishRead;
    liftStruct* thisLift = (liftStruct*)args;
    pthread_mutex_lock(thisLift->mutexLock);
    localFinishRead = *(thisLift->finishedRead);
    pthread_mutex_unlock(thisLift->mutexLock);
    while (localFinishRead != TRUE) {
        pthread_mutex_lock(thisLift->mutexLock);
        if (thisLift->buffer->list->size < 1 && *(thisLift->finishedRead) != TRUE) {
            #ifdef DEBUG
            printf("THREAD %ld (AKA: LIFT %d): Signaling condition empty in lifts.c (localFinishedRead = %d)\n", pthread_self(), thisLift->liftNumber, localFinishRead);
            #endif
            pthread_cond_signal(thisLift->empty);
        } else {
            currentRequest = dequeue(thisLift->buffer);
            if (currentRequest != NULL) {
                printf("----LIFT %d----\nGoing from Floor %d to Floor %d\n--------------\n",
                thisLift->liftNumber, ((request*)currentRequest)->requestFloor,
                ((request*)currentRequest)->destinationFloor);
                free(currentRequest);
            } else {
                #ifdef DEBUG
                printf("\n!!!! RECIEVED A NULL ON THREAD %ld (AKA: LIFT %d)!!!!\n", pthread_self(), thisLift->liftNumber);
                #endif
                /* This occurs once at the end and doesnt happen again as it hasnt updated localFinishRead. */
            }
        }
        localFinishRead = *(thisLift->finishedRead);
        pthread_mutex_unlock(thisLift->mutexLock);
    }
    return NULL;
}

liftStruct* initLiftStruct(queue* inBuffer, int inTimer, int whatLift,
pthread_mutex_t* inLock, pthread_cond_t* inFullCond, pthread_cond_t* inEmptyCond,
int* inFinishedRead, int inMaxBufferSize) {
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
    return newLiftStruct;
}

void freeLiftStruct(liftStruct* toFree) {
    free(toFree);
}
