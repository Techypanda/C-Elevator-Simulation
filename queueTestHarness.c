#include "queue.h"
#include <stdio.h>
int main() {
    int i;
    queue* myQueue = createQueue();
    for (i = 0; i < 100; i++) {
        enqueue(i, myQueue);
    }
    printf("%d ", dequeue(myQueue));
    for (i = 0; i < 99; i++) {
        printf(" %d", dequeue(myQueue));
    }
    printf("\n");
}