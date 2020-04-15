#include <pthread.h>
#include "queue.h"
#include "request.h"
#ifndef LIFTS_H
#define LIFTS_H
#define TRUE 1
#define FALSE 0
/*typedef struct liftStruct {
    queue* buffer;
    int* remainingSemaphore;
    int* countSemaphore;
    int* finishedRead;
    pthread_mutex_t mutexLock;
    int liftTimer;
    pthread_t* FirstThread;
    pthread_t* SecondThread;
    pthread_t* ThirdThread;
    pthread_cond_t full;
    pthread_cond_t empty;
} liftStruct;
liftStruct* initLiftStruct(queue* inBuffer, int inRemaining, int inCount, int inTime, pthread_t* inFirst, pthread_t* inSecond, pthread_t* inThird);
*/
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
} liftStruct;
liftStruct* initLiftStruct(queue* inBuffer, int inTimer, int whatLift,
pthread_mutex_t* inLock, pthread_cond_t* inFullCond, pthread_cond_t* inEmptyCond,
int* inFinishedRead, int inMaxBufferSize);
void freeLiftStruct(liftStruct* toFree);
void* lift(void* args);
#endif
