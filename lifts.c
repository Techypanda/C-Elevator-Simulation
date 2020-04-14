#include <stdlib.h>
#include <stdio.h>
#include "lifts.h"
#include "request.h"

void* lift(void* args) {
    request* currentRequest;
    liftStruct* myLift = (liftStruct*)args;
    /*if (pthread_self() == *(myLift->FirstThread)) {
        printf("----Lift One----\n");
    } else if (pthread_self() == *(myLift->SecondThread)) {
        printf("----Lift Two----\n");
    } else if (pthread_self() == *(myLift->ThirdThread)) {
        printf("----Lift Three----\n");
    }*/
    while ((*(myLift->finishedRead)) == 0 || (*(myLift->countSemaphore)) != 0) {
        if ((*(myLift->mutexLock)) == 0) {
            (*(myLift->mutexLock)) = 1;
            /* CRITICAL SECTION */
            if ((*(myLift->countSemaphore)) > 0) {
                (*(myLift->countSemaphore)) -= 1;
                (*(myLift->remainingSemaphore)) += 1;
                currentRequest = dequeue(myLift->buffer);
                printf("Going from Floor %d to Floor %d\n", currentRequest->requestFloor, currentRequest->destinationFloor);
                free(currentRequest);
            }
            (*(myLift->mutexLock)) = 0;
        }
    }
    return NULL;
}

liftStruct* initLiftStruct(queue* inBuffer, int inRemaining, int inCount, int inTime,
pthread_t* inFirst, pthread_t* inSecond, pthread_t* inThird) {
    liftStruct* myData = (liftStruct*)malloc(sizeof(liftStruct));
    myData->buffer = inBuffer;
    myData->remainingSemaphore = &inRemaining;
    myData->countSemaphore = &inCount;
    myData->finishedRead = (int*)malloc(sizeof(int));
    myData->mutexLock = (int*)malloc(sizeof(int));
    *(myData->finishedRead) = 0;
    *(myData->mutexLock) = 0;
    myData->liftTimer = inTime;
    myData->FirstThread = inFirst;
    myData->SecondThread = inSecond;
    myData->ThirdThread = inThird;
    return myData;
}

void freeLiftStruct(liftStruct* toFree) {
    free(toFree->finishedRead);
    free(toFree->mutexLock);
    free(toFree);
}
