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

void* requestt(void* args) {
    FILE* file = fopen("sim_input", "r");
    int destination, from, fscanfReturn;
    liftStruct* myLift = (liftStruct*)args;
    fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
    while(fscanfReturn != EOF) { /* Keep Going Till End Of File */
        if (myLift->remainingSemaphore != 0) {
            enqueue(createRequest(from, destination), myLift->buffer);
            fscanfReturn = fscanf(file, "%d %d\n", &from, &destination);
            myLift->remainingSemaphore -= 1;
            myLift->countSemaphore += 1;
        } /* ELSE THEN WAIT */
    }
    fclose(file);
    myLift->finishedRead = 1;
    return NULL;
}
