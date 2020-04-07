#ifndef QUEUE_H
#define QUEUE_H
typedef struct listNode {
    void* data;
    struct listNode* next;
} listNode;
typedef struct queue {
    listNode* head;
    listNode* tail;
} queue;
queue* newQueue();
/* listNode* createNode(void* data); PRIVATE */
void addItem(queue* inQueue, void* inData);
void removeItem(queue* inQueue);
#endif