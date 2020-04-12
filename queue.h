#include "list.h"
#ifndef QUEUE_H
#define QUEUE_H
typedef struct queue {
    linkedList* list;
} queue;
queue* createQueue();
void freeQueue(queue* myQueue);
void enqueue(void* inObject, queue* myQueue);
void* dequeue(queue* myQueue);
void* peek(queue* myQueue);
#endif