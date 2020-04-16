#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
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

void liftProcess(void* args) {
    struct timespec tims;
    #ifdef DEBUG
    int tester;
    #endif
    #ifndef NOTSLEEP
    int localSleepTimer;
    #endif
    int distance, sleepNeeded;
    processLift** thisLift;
    request currentRequest;
    thisLift = (processLift**)args;
    #ifndef NOTSLEEP
    localSleepTimer = (*thisLift)->liftTimer;
    #endif
    while (**((*thisLift)->finishedRead) == FALSE || (*(*thisLift)->buffer)->size != 0) {
        #ifdef DEBUG
        sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
        printf("FULL SEM VALUE BEFORE: %d\n", tester);
        #endif
        clock_gettime(CLOCK_REALTIME, &tims);
        tims.tv_sec += 1; /* after 4 seconds, check the file hasnt been finished read. */
        if (sem_timedwait(*((*thisLift)->semaphoreFull), &tims) == 0 && (*(*thisLift)->buffer)->size != 0) {
            #ifdef DEBUG
            sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
            printf("FULL SEM VALUE AFTER: %d\n", tester);
            #endif
            currentRequest = arrayDequeue((*thisLift)->buffer);
            if (((*thisLift)->previousRequest).destinationFloor == -1) { /* Equivalent of Not Null from the thread implementation, however, I used a stack variable so i just set it to -1. */
                distance = abs(1 - currentRequest.requestFloor) + abs(currentRequest.destinationFloor - currentRequest.requestFloor);
                /*((thisLift->liftReturnVals) + 0) += distance;
                *((thisLift->liftReturnVals) + 1) += 1;*/
                sem_wait(*((*thisLift)->fileSem));
                fprintf(**((*thisLift)->out_sim_file),
                    "Lift-%d Operation\nPrevious position: Floor 1 (I haven't had any requests yet.)\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                    (*thisLift)->liftNumber, currentRequest.requestFloor, currentRequest.destinationFloor, 1, currentRequest.requestFloor, currentRequest.requestFloor, currentRequest.destinationFloor,
                    distance, 1, distance, currentRequest.requestFloor
                );
                sem_post(*((*thisLift)->fileSem));
                (*thisLift)->previousRequest = currentRequest;
                #ifndef NOTSLEEP
                sleepNeeded = TRUE;
                #endif
            } else {
                distance = abs(((*thisLift)->previousRequest).destinationFloor - currentRequest.requestFloor) + abs(currentRequest.destinationFloor - currentRequest.requestFloor);
                /*((thisLift->liftReturnVals) + 0) += distance;
                *((thisLift->liftReturnVals) + 1) += 1;*/
                sem_wait(*((*thisLift)->fileSem));
                fprintf(**((*thisLift)->out_sim_file),
                    "Lift-%d Operation\nPrevious position: Floor %d\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                    (*thisLift)->liftNumber, ((*thisLift)->previousRequest).destinationFloor, currentRequest.requestFloor, currentRequest.destinationFloor, ((*thisLift)->previousRequest).destinationFloor, currentRequest.requestFloor,
                    currentRequest.requestFloor, currentRequest.destinationFloor, distance, 1, distance, currentRequest.requestFloor
                );
                sem_post(*((*thisLift)->fileSem));
                (*thisLift)->previousRequest = currentRequest;
                #ifndef NOTSLEEP
                sleepNeeded = TRUE;
                #endif
            }


            #ifdef DEBUG
            sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
            printf("EMPTY SEM VALUE BEFORE: %d\n", tester);
            #endif
            sem_post(*((*thisLift)->semaphoreEmpty));
            #ifdef DEBUG
            sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
            printf("EMPTY SEM VALUE AFTER: %d\n", tester);
            #endif
            if (sleepNeeded == TRUE) {
                #ifndef NOTSLEEP
                sleepNeeded = FALSE;
                sleep(localSleepTimer);
                #endif
            }
        } else {
            sem_post(*((*thisLift)->semaphoreFull));
            /* Check the initial condition again! */
        }
    }
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
    return newLiftStruct;
}

void freeLiftStruct(liftStruct* toFree) {
    free(toFree->liftReturnVals);
    free(toFree);
}

processLift* createProcessLift(arrayQueue** inBuffer, int** inFinishedRead, int inTimer,
int inNumber, int myCapacity, FILE*** inFile, sem_t** inFullSem, sem_t** inEmptySem,
sem_t** inFileSem) {
    request nullValue;
    processLift* myLift = (processLift*)malloc(sizeof(processLift));
    nullValue.requestFloor = -1; nullValue.destinationFloor = -1;
    myLift->buffer = inBuffer;
    myLift->previousRequest = nullValue;
    myLift->finishedRead = inFinishedRead;
    myLift->liftReturnVals = -1;
    myLift->liftTimer = inTimer;
    myLift->liftNumber = inNumber;
    myLift->maxBufferSize = myCapacity;
    myLift->out_sim_file = inFile;
    myLift->semaphoreFull = inFullSem;
    myLift->semaphoreEmpty = inEmptySem;
    myLift->fileSem = inFileSem;
    return myLift;
}
