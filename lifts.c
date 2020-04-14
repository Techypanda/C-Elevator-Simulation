#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lifts.h"
#include "request.h"

void* lift(void* args) {
    request* currentRequest;
    liftStruct* myLift = (liftStruct*)args;
    pthread_mutex_lock(&(myLift->mutexLock));
    while ((*(myLift->finishedRead)) == 0) {
        #ifdef DEBUG
        printf("THREAD %ld: Waiting On Condition full in lifts.c\n", pthread_self());
        #endif
        pthread_cond_wait((&(myLift->full)), &(myLift->mutexLock));
        while ((*(myLift->countSemaphore)) > 0) {
            (*(myLift->countSemaphore)) -= 1;
            (*(myLift->remainingSemaphore)) += 1;
            currentRequest = dequeue(myLift->buffer);
            printf("Going from Floor %d to Floor %d\n", currentRequest->requestFloor, currentRequest->destinationFloor);
            free(currentRequest);
        }
        #ifdef DEBUG
        printf("THREAD %ld: Signaling condition empty in lifts.c\n", pthread_self());
        #endif
        pthread_cond_signal(&(myLift->empty));
    }
    pthread_mutex_unlock(&(myLift->mutexLock));
    #ifdef DEBUG
    printf("THREAD: %ld HAS FINISHED\n", pthread_self());
    #endif
    pthread_cond_signal(&(myLift->full));
    return NULL;
}

liftStruct* initLiftStruct(queue* inBuffer, int inRemaining, int inCount, int inTime,
pthread_t* inFirst, pthread_t* inSecond, pthread_t* inThird) {
    pthread_mutexattr_t canShare;
    liftStruct* myData = (liftStruct*)malloc(sizeof(liftStruct));
    myData->buffer = inBuffer;
    myData->remainingSemaphore = (int*)malloc(sizeof(int));
    myData->finishedRead = (int*)malloc(sizeof(int));
    myData->countSemaphore = (int*)malloc(sizeof(int));
    *(myData->remainingSemaphore) = inRemaining;
    *(myData->finishedRead) = 0;
    *(myData->countSemaphore) = inCount;
    myData->liftTimer = inTime;
    myData->FirstThread = inFirst;
    myData->SecondThread = inSecond;
    myData->ThirdThread = inThird;
    pthread_mutexattr_init(&canShare);
    pthread_mutexattr_setpshared(&canShare, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(myData->mutexLock), &canShare);
    pthread_cond_init(&(myData->full), NULL);
    pthread_cond_init(&(myData->empty), NULL);
    return myData;
}

void freeLiftStruct(liftStruct* toFree) {
    free(toFree->finishedRead);
    free(toFree->remainingSemaphore);
    free(toFree->countSemaphore);
    free(toFree);
}
