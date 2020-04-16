#ifndef PROCESSLIFT_H
#define PROCESSLIFT_H
#include <semaphore.h>
typedef struct arrayQueue {
    int front;
    int back;
    int capacity;
    int size;
    request** myBuffer;
} arrayQueue;
typedef struct processLift {
    arrayQueue** buffer;
    request previousRequest;
    int** finishedRead;
    int liftReturnVals; /* [0] = #movement, [1] = #requests */
    int liftTimer;
    int liftNumber;
    int maxBufferSize;
    FILE** out_sim_file;
    sem_t* semaphoreFull;
    sem_t* semaphoreEmpty;
} processLift;
#endif
