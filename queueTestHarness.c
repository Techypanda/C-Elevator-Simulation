#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
    char* a; int i; queue* newQueue;
    queue* myQueue = createQueue();
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
    newQueue = createQueue();
    for (i = 0; i < 50; i++) {
        a = malloc(sizeof(char));
        enqueue(a, newQueue);
    }
    freeQueue(newQueue, &free);
    return(0);
}
