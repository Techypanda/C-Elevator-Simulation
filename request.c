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

/*void* requestt(void* args) {
    int destination, from, fscanfReturn;
    queue* buffer = (queue*)args;
    FILE* file = fopen("sim_input", "r");
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while(fscanfReturn != EOF) {
        if (buffer->list->size < buffer->bufferSize) {
            enqueue(createRequest(from, destination), buffer);
            fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
        }*/ /* Waiting... */
    /*}
    fclose(file);
    return NULL;
} OLD BEFORE SEMAPHORE IMPLEMENTATION*/

/*void* requestt(void* args) {
    FILE* file = fopen("sim_input", "r");
    int destination, from, fscanfReturn;
    liftStruct* myLift = (liftStruct*)args;
    pthread_mutex_lock(&(myLift->mutexLock));
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while(fscanfReturn != EOF) {
        if ((*(myLift->remainingSemaphore)) > 0) {
            enqueue(createRequest(from, destination), myLift->buffer);
            fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
            (*(myLift->remainingSemaphore)) -= 1;
            (*(myLift->countSemaphore)) += 1;
        }
    }
    fclose(file);
    (*(myLift->finishedRead)) = 1;
    pthread_mutex_unlock(&(myLift->mutexLock));
    return NULL;
}*/

void* requestt(void* args) {
    FILE* file = fopen("sim_input", "r");
    int destination, from, fscanfReturn;
    liftStruct* myLift = (liftStruct*)args;
    pthread_mutex_lock(&(myLift->mutexLock));
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while (fscanfReturn != EOF) {
        while ((*(myLift->countSemaphore)) > 0) {
            #ifdef DEBUG
            printf("THREAD %ld: Waiting On Condition empty in request.c\n", pthread_self());
            #endif
            pthread_cond_wait(&(myLift->empty), &(myLift->mutexLock));
        }
        while (fscanfReturn != EOF && (*(myLift->remainingSemaphore)) > 0) {
            enqueue(createRequest(from, destination), myLift->buffer);
            fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
            (*(myLift->remainingSemaphore)) -= 1;
            (*(myLift->countSemaphore)) += 1;
        }
        #ifdef DEBUG
        printf("THREAD %ld: Signaling Condition full in request.c\n", pthread_self());
        #endif
        pthread_cond_signal((&(myLift->full)));
    }
    (*(myLift->finishedRead)) = 1;
    pthread_mutex_unlock(&(myLift->mutexLock));
    #ifdef DEBUG
    printf("Signaling Condition full in request END OF LINE!!.c\n");
    #endif
    pthread_cond_signal((&(myLift->full)));
    fclose(file);
    return NULL;
}
