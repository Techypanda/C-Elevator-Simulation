#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
    int i; char* a; queue* newQueue;
    queue* myQueue = createQueue(9999);
    for (i = 0; i < 100; i++) {
        enqueue(&i, myQueue);
    }
    printf("%d ", *((int*)dequeue(myQueue)));
    for (i = 0; i < 99; i++) {
        printf(" %d", *((int*)dequeue(myQueue)));
    }
    printf("\n");
    freeQueue(myQueue, &free);
    /* ^^^ Passed with no leaks. */
    newQueue = createQueue(9999);
    for (i = 0; i < 50; i++) {
        a = malloc(sizeof(char));
        enqueue(a, newQueue);
    }
    freeQueue(newQueue, &free);
    return(0);
}
