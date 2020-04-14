#include <stdlib.h>
#include "queue.h"
#include "list.h"
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
