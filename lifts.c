#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"
#include "lifts.h"
#include "request.h"

/*******************************************************************************
*   PURPOSE OF C FILE: Provides Implementation of how the lifts will work.
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
/*******************************************************************************
*   PURPOSE: This method is the PThread function for the actual lifts going up/down,
*   it takes in a struct which points at memory such as mutexLock which allows it
*   to avoid race conditions.
*   IMPORTS: liftStruct* lift (fed in as a void* due to being a pthread.)
*   EXPORTS: NULL, (the lift's actual returns (request # and movement is done
*   in a array contained in the struct.))
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
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

/*******************************************************************************
*   PURPOSE: This is the process implementation of the actual lifts going up and
*   down, this uses semaphores fed in through a struct.
*   IMPORTS: processLift** thisLift (fed in as void* args because I originally
*   thought that processes ran functions the same way pthreads did.)
*   EXPORTS: NULL (Exports again happen through a array contained in the struct.)
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void liftProcess(void* args) {
    struct timespec tims;
    #ifdef DEBUG
    int tester;
    #endif
    #ifndef NOTSLEEP
    int localSleepTimer, localCount;
    #endif
    int distance, sleepNeeded;
    processLift** thisLift;
    request currentRequest;
    localCount = 0;
    thisLift = (processLift**)args;
    #ifndef NOTSLEEP
    localSleepTimer = (*thisLift)->liftTimer;
    #endif
    while (**((*thisLift)->finishedRead) == FALSE || localCount != 0) {
        #ifdef DEBUG
        sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
        printf("FULL SEM VALUE BEFORE: %d\n", tester);
        #endif
        clock_gettime(CLOCK_REALTIME, &tims);
        tims.tv_sec += 1; /* after 4 seconds, check the file hasnt been finished read. */
        if (sem_timedwait(*((*thisLift)->semaphoreFull), &tims) == 0) {
            #ifdef DEBUG
            sem_getvalue( *((*thisLift)->semaphoreFull), &tester);
            printf("FULL SEM VALUE AFTER: %d\n", tester);
            #endif
            localCount =(*(*thisLift)->buffer)->size;
            if (localCount > 0) {
                currentRequest = arrayDequeue((*thisLift)->buffer);
                if (((*thisLift)->previousRequest).destinationFloor == -1) { /* Equivalent of Not Null from the thread implementation, however, I used a stack variable so i just set it to -1. */
                    distance = abs(1 - currentRequest.requestFloor) + abs(currentRequest.destinationFloor - currentRequest.requestFloor);
                    *((*((*thisLift)->liftReturnVals)) + 0) += distance;
                    *((*((*thisLift)->liftReturnVals)) + 1) += 1;
                    sem_wait(*((*thisLift)->requestFileSem));
                    fprintf(**((*thisLift)->out_sim_file),
                        "Lift-%d Operation\nPrevious position: Floor 1 (I haven't had any requests yet.)\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                        (*thisLift)->liftNumber, currentRequest.requestFloor, currentRequest.destinationFloor, 1, currentRequest.requestFloor, currentRequest.requestFloor, currentRequest.destinationFloor,
                        distance, *((*((*thisLift)->liftReturnVals)) + 1), *((*((*thisLift)->liftReturnVals)) + 0), currentRequest.requestFloor
                    );
                    fflush(**((*thisLift)->out_sim_file));
                    (*thisLift)->previousRequest = currentRequest;
                    #ifndef NOTSLEEP
                    sleepNeeded = TRUE;
                    #endif
                    sem_post(*((*thisLift)->liftZeroFileSem));
                } else {
                    distance = abs(((*thisLift)->previousRequest).destinationFloor - currentRequest.requestFloor) + abs(currentRequest.destinationFloor - currentRequest.requestFloor);
                    *((*((*thisLift)->liftReturnVals)) + 0) += distance;
                    *((*((*thisLift)->liftReturnVals)) + 1) += 1;
                    #ifdef DEBUG
                    sem_getvalue( *((*thisLift)->requestFileSem), &tester);
                    printf("REQUEST FILE SEM VALUE BEFORE END: %d\n", tester);
                    #endif
                    sem_wait(*((*thisLift)->requestFileSem));
                    fprintf(**((*thisLift)->out_sim_file),
                        "Lift-%d Operation\nPrevious position: Floor %d\nRequest: Floor %d to Floor %d\nDetail operations:\n    Go from Floor %d to Floor %d\n    Go from Floor %d to Floor %d\n    #movement for this request: %d\n    #request: %d\n    Total #movement: %d\nCurrent position: Floor %d\n\n",
                        (*thisLift)->liftNumber, ((*thisLift)->previousRequest).destinationFloor, currentRequest.requestFloor, currentRequest.destinationFloor, ((*thisLift)->previousRequest).destinationFloor, currentRequest.requestFloor,
                        currentRequest.requestFloor, currentRequest.destinationFloor, distance, *((*((*thisLift)->liftReturnVals)) + 1), *((*((*thisLift)->liftReturnVals)) + 0), currentRequest.requestFloor
                    );
                    fflush(**((*thisLift)->out_sim_file));
                    (*thisLift)->previousRequest = currentRequest;
                    #ifndef NOTSLEEP
                    sleepNeeded = TRUE;
                    #endif
                    sem_post(*((*thisLift)->liftZeroFileSem));
                }
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

/*******************************************************************************
*   PURPOSE: This initializes the struct used by lifts in the pthreads implementation
*   IMPORTS: queue* inBuffer, int inTimer, int whatLift, pthread_mutex_t* inLock
*   pthread_cond_t* inFullCond, pthread_cond_t* inEmptyCond,
*   int* inFinishedRead, int inMaxBufferSize, FILE* inFile
*   EXPORTS: liftStruct* newLiftStruct
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
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

/*******************************************************************************
*   PURPOSE: Will free the memory allocated for a liftStruct.
*   IMPORTS: liftStruct* toFree
*   EXPORTS: None
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void freeLiftStruct(liftStruct* toFree) {
    free(toFree->liftReturnVals);
    free(toFree);
}

/*******************************************************************************
*   PURPOSE: Will create a lift for the process implementation.
*   IMPORTS: arrayQueue** inBuffer, int** inFinishedRead, int inTimer,
*   int inNumber, int myCapacity, FILE*** inFile, sem_t** inFullSem, sem_t** inEmptySem,
*   sem_t** inFileSem, sem_t** inRequestFileSem, int** inReturnAddress
*   EXPORTS: processLift* myLift
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
processLift* createProcessLift(arrayQueue** inBuffer, int** inFinishedRead, int inTimer,
int inNumber, int myCapacity, FILE*** inFile, sem_t** inFullSem, sem_t** inEmptySem,
sem_t** inFileSem, sem_t** inRequestFileSem, int** inReturnAddress) {
    request nullValue;
    processLift* myLift = (processLift*)malloc(sizeof(processLift));
    nullValue.requestFloor = -1; nullValue.destinationFloor = -1;
    myLift->buffer = inBuffer;
    myLift->previousRequest = nullValue;
    myLift->finishedRead = inFinishedRead;
    myLift->liftReturnVals = inReturnAddress;
    myLift->liftTimer = inTimer;
    myLift->liftNumber = inNumber;
    myLift->maxBufferSize = myCapacity;
    myLift->out_sim_file = inFile;
    myLift->semaphoreFull = inFullSem;
    myLift->semaphoreEmpty = inEmptySem;
    myLift->liftZeroFileSem = inFileSem;
    myLift->requestFileSem = inRequestFileSem;
    return myLift;
}
