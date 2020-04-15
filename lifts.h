#include <pthread.h>
#include <stdio.h>
#include "queue.h"
#include "request.h"
#ifndef LIFTS_H
#define LIFTS_H
#define TRUE 1
#define FALSE 0
typedef struct liftStruct { /* Lift One */
    queue* buffer;
    request* previousRequest;
    int* finishedRead;
    int liftTimer;
    int liftNumber;
    int maxBufferSize;
    pthread_mutex_t* mutexLock;
    pthread_cond_t* full;
    pthread_cond_t* empty;
    FILE* out_sim_file;
} liftStruct;
liftStruct* initLiftStruct(queue* inBuffer, int inTimer, int whatLift,
pthread_mutex_t* inLock, pthread_cond_t* inFullCond, pthread_cond_t* inEmptyCond,
int* inFinishedRead, int inMaxBufferSize, FILE* inFile);
void freeLiftStruct(liftStruct* toFree);
void* lift(void* args);
#endif
