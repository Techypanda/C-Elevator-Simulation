#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include "queue.h"
#include "list.h"
#include "request.h"
/* LIST BASED */
queue* createQueue(int inBuffer) {
    queue* newQueue = (queue*)malloc(sizeof(queue));
    newQueue->list = createLinkedList();
    newQueue->bufferSize = inBuffer;
    return newQueue;
}
void freeQueue(queue* myQueue, void (*freeMethod)(void* inData)) {
    freeLinkedList(myQueue->list, freeMethod);
    free(myQueue);
}
void enqueue(void* inObject, queue* myQueue) {
    insertStart(myQueue->list, inObject);
}
void* dequeue(queue* myQueue) {
    return removeLast(myQueue->list);
}
void* peek(queue* myQueue) {
    return myQueue->list->tail;
}

/* ARRAY BASED SHUFFLING QUEUE */
/* This code is possibly the worst i've ever written
Good god did this drive me insane
the hours i spent printing out memory addresses to figure out why the fuck
for it to just be a logic error
i hate c
i used to like c
now i hate it */
arrayQueue* createArrayQueue(request** inBuffer, int inBufferSize) {
    arrayQueue* myQueue = (arrayQueue*)mmap(NULL,sizeof(arrayQueue),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    myQueue->myBuffer = inBuffer;
    myQueue->front = 0;
    myQueue->back = myQueue->front;
    myQueue->capacity = inBufferSize;
    myQueue->size = 0;
    return myQueue;
}
request arrayDequeue(arrayQueue** inBuffer) {
    request outRequest, temp;
    outRequest.destinationFloor = -1;
    outRequest.requestFloor = -1;
    temp.destinationFloor = -1;
    temp.requestFloor = -1;
    if ((*inBuffer)->size != 0) {
        outRequest = ((*((*inBuffer)->myBuffer))[0]);
        shuffle((*inBuffer)->myBuffer, (*inBuffer)->back - 1);
        if ((*inBuffer)->back < (*inBuffer)->capacity) {
            ((*((*inBuffer)->myBuffer))[(*inBuffer)->back]) = temp;
        }
        (*inBuffer)->back -= 1;
        (*inBuffer)->size -= 1;
    }
    else {
        fprintf(stderr, "Unable to dequeue. Array is empty.\n");
    }
    return outRequest;
}
void arrayEnqueue(request inData, arrayQueue** inBuffer) {
    if ((*inBuffer)->capacity == (*inBuffer)->size) {
        fprintf(stderr, "Unable to enqueue, Array is full.\n");
    } else {
        (*((*inBuffer)->myBuffer))[(*inBuffer)->back] = inData;
        (*inBuffer)->back += 1;
        (*inBuffer)->size += 1;
    }
}
void shuffle(request** inArray, int whereBackIs) {
    int k;
    for (k = 0; k < whereBackIs; k++) {
        (*inArray)[k] = (*inArray)[k + 1];
    }
}

void cleanupArrayQueue(arrayQueue** myAddr) {
    munmap(*myAddr, sizeof(arrayQueue));
}
