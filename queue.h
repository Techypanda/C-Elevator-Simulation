#include "list.h"
#ifndef QUEUE_H
#define QUEUE_H
typedef struct queue {
    linkedList* list;
    int bufferSize;
} queue;
queue* createQueue(int bufferSize);
void freeQueue(queue* myQueue, void (*freeMethod)(void* inData));
void enqueue(void* inObject, queue* myQueue);
void* dequeue(queue* myQueue);
void* peek(queue* myQueue);
#endif
