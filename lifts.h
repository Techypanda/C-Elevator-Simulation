#include <pthread.h>
#include "queue.h"
#ifndef LIFTS_H
#define LIFTS_H
typedef struct liftStruct {
    queue* buffer;
    int remainingSemaphore; /* Count number of remaining slots in buffer. */
    int countSemaphore; /* Count number of spots taken in buffer */
    int finishedRead; /* Boolean to say if they have finished reading the sim_input */
    int mutexLock; /* Make sure only one thread can read the buffer at a time */
    int liftTimer;
    pthread_t* FirstThread; /* LIFT ONE */
    pthread_t* SecondThread; /* LIFT TWO */
    pthread_t* ThirdThread; /* LIFT THREE */
} liftStruct;
liftStruct* initLiftStruct(queue* inBuffer, int inRemaining, int inCount, int inTime, pthread_t* inFirst, pthread_t* inSecond, pthread_t* inThird);
void* lift(void* args);
#endif
