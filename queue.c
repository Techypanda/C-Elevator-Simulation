#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdio.h>
#include "queue.h"
#include "list.h"
#include "request.h"
/* LIST BASED */
/*******************************************************************************
*   PURPOSE: To create a list based queue.
*   IMPORTS: int inBuffer (Not actually needed, its a list but I included it)
*   EXPORTS: queue* newQueue
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
queue* createQueue(int inBuffer) {
    queue* newQueue = (queue*)malloc(sizeof(queue));
    newQueue->list = createLinkedList();
    newQueue->bufferSize = inBuffer;
    return newQueue;
}
/*******************************************************************************
*   PURPOSE: frees the queue and the list.
*   IMPORTS: queue* myQueue and the Free method
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void freeQueue(queue* myQueue, void (*freeMethod)(void* inData)) {
    freeLinkedList(myQueue->list, freeMethod);
    free(myQueue);
}
/*******************************************************************************
*   PURPOSE: enqueues objects into the queue
*   IMPORTS: void* inObject, queue* myQueue
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void enqueue(void* inObject, queue* myQueue) {
    insertStart(myQueue->list, inObject);
}
/*******************************************************************************
*   PURPOSE: dequeue objects from the queue
*   IMPORTS: queue* myQueue
*   EXPORTS: void* outObject
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void* dequeue(queue* myQueue) {
    return removeLast(myQueue->list);
}
/*******************************************************************************
*   PURPOSE: returns the object at the top of the queue but does not free it from
*   list.
*   IMPORTS: queue* myQueue
*   EXPORTS: void* outObject
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void* peek(queue* myQueue) {
    return myQueue->list->tail;
}

/* ARRAY BASED SHUFFLING QUEUE */
/*******************************************************************************
*   PURPOSE: Creates an array based queue.
*   IMPORTS: request** inBuffer, int inBufferSize
*   EXPORTS: arrayQueue* outQueue
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
arrayQueue* createArrayQueue(request** inBuffer, int inBufferSize) {
    arrayQueue* myQueue = (arrayQueue*)mmap(NULL,sizeof(arrayQueue),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    myQueue->myBuffer = inBuffer;
    myQueue->front = 0;
    myQueue->back = myQueue->front;
    myQueue->capacity = inBufferSize;
    myQueue->size = 0;
    return myQueue;
}
/*******************************************************************************
*   PURPOSE: Dequeues an object from the array based queue.
*   IMPORTS: arrayQueue** inBuffer
*   EXPORTS: request outRequest
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
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
/*******************************************************************************
*   PURPOSE: Enqueue an object into an array based queue.
*   IMPORTS: request inData, arrayQueue** inBuffer
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void arrayEnqueue(request inData, arrayQueue** inBuffer) {
    if ((*inBuffer)->capacity == (*inBuffer)->size) {
        fprintf(stderr, "Unable to enqueue, Array is full.\n");
    } else {
        (*((*inBuffer)->myBuffer))[(*inBuffer)->back] = inData;
        (*inBuffer)->back += 1;
        (*inBuffer)->size += 1;
    }
}
/*******************************************************************************
*   PURPOSE: Shuffles the array in the queue.
*   IMPORTS: request** inArray, int whereBackIs
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void shuffle(request** inArray, int whereBackIs) {
    int k;
    for (k = 0; k < whereBackIs; k++) {
        (*inArray)[k] = (*inArray)[k + 1];
    }
}
/*******************************************************************************
*   PURPOSE: Removes the memory mapping of the arrayQueue from shared memory.
*   IMPORTS: arrayQueue** myAddr
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
void cleanupArrayQueue(arrayQueue** myAddr) {
    munmap(*myAddr, sizeof(arrayQueue));
}
