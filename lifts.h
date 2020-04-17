 #ifndef LIFTS_H
#define LIFTS_H
#define TRUE 1
#define FALSE 0
#define CALCDISTANCE(a,b,c,d) ((a - b) + (c - d))
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include "queue.h"
#include "request.h"
#include "processLift.h"
/*******************************************************************************
*   PURPOSE: This is the struct for each lift in the PThread implementation
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
typedef struct liftStruct { /* Lift One */
    queue* buffer;
    request* previousRequest;
    int* finishedRead;
    int* liftReturnVals; /* [0] = #movement, [1] = #requests */
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
void liftProcess(void* args);
processLift* createProcessLift(arrayQueue** inBuffer, int** inFinishedRead, int inTimer,
int inNumber, int myCapacity, FILE*** inFile, sem_t** inFullSem, sem_t** inEmptySem,
sem_t** inFileSem, sem_t** inRequestFileSem, int** inReturnAddress);
#endif
