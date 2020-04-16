#ifndef QUEUE_H
#define QUEUE_H
#include "request.h"
#include "list.h"
#include "processLift.h"
typedef struct queue {
    linkedList* list;
    int bufferSize;
} queue;
queue* createQueue(int bufferSize);
void freeQueue(queue* myQueue, void (*freeMethod)(void* inData));
void enqueue(void* inObject, queue* myQueue);
void* dequeue(queue* myQueue);
void* peek(queue* myQueue);

arrayQueue* createArrayQueue(request** inBuffer, int inBufferSize);
void arrayEnqueue(request inRequest, arrayQueue** inBuffer);
request arrayDequeue(arrayQueue** inBuffer);
void shuffle(request** arrayToShuffle, int whereBackIs);
void cleanupArrayQueue(arrayQueue** myAddr);
#endif
