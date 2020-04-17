#ifndef PROCESSLIFT_H
#define PROCESSLIFT_H
#include <semaphore.h>
/*******************************************************************************
*   PURPOSE: Queue ADT with a array data structure
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
typedef struct arrayQueue {
    int front;
    int back;
    int capacity;
    int size;
    request** myBuffer;
} arrayQueue;
/*******************************************************************************
*   PURPOSE: Lift Struct for each Lift in the Process Implementation.
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
typedef struct processLift {
    arrayQueue** buffer;
    request previousRequest;
    int** finishedRead;
    int** liftReturnVals; /* [0] = #movement, [1] = #requests */
    int liftTimer;
    int liftNumber;
    int maxBufferSize;
    FILE*** out_sim_file;
    sem_t** semaphoreFull;
    sem_t** semaphoreEmpty;
    sem_t** liftZeroFileSem;
    sem_t** requestFileSem;
} processLift;
#endif
