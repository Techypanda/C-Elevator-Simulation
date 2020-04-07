#include <stdlib.h>
#include "queue.h"
/* THIS IS A PRIVATE METHOD, IT SHOULD NOT BE USED OUTSIDE OF THIS
C FILE */
listNode* createNode(void* inData) {
    listNode* newNode = malloc(sizeof(listNode));
    newNode->data = inData;
    newNode->next = NULL;
    return newNode;
    /*https://www.geeksforgeeks.org/queue-linked-list-implementation/*/
}
queue* newQueue() {

}
