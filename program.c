#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#ifdef PTHREAD
#include <pthread.h>
#endif
#ifdef PROCESS
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#endif
#include "lifts.h"
#include "queue.h"
#include "program.h"
#include "request.h"
#include "list.h"

/*******************************************************************************
*   PURPOSE: Starting point for program.
*   IMPORTS: int argc, char** argv
*   EXPORTS: int errorStatus
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
int main(int argc, char** argv) {
    char* endCharacter;
    int bufferSize, liftTime;
    if (argc == 3) {
        bufferSize = strtol(argv[1], &endCharacter, 10);
            /* This Error Check is from the Linux C Man on strtol */
        if ((errno == ERANGE && (bufferSize == LONG_MAX || bufferSize == LONG_MIN)) || (errno != 0 && bufferSize == 0)) {
            fprintf(stderr, "Invalid Integer Entered for bufferSize: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        /* This Error Check is from the Linux C Man on strtol */
        if (endCharacter == argv[1]) {
            fprintf(stderr, "No Digits were found for m.\n");
            exit(EXIT_FAILURE);
        }
        /* This Error Check is from the Linux C Man on strtol */
        if (*endCharacter != '\0')
            printf("Only integers can be entered, decimal point has been cut off for buffer.\n");
        liftTime = strtol(argv[2], &endCharacter, 10);
        /* This Error Check is from the Linux C Man on strtol */
        if (endCharacter == argv[2]) {
            fprintf(stderr, "No Digits were found for t.\n");
            exit(EXIT_FAILURE);
        }
        /* This Error Check is from the Linux C Man on strtol */
        if (*endCharacter != '\0')
            printf("Only integers can be entered, decimal point has been cut off for time.\n");
        if ((errno == ERANGE && (liftTime == LONG_MAX || liftTime == LONG_MIN)) || (errno != 0 && liftTime == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        #ifdef DEBUG
        printf("Reached with values m:%d and t:%d\n",bufferSize,liftTime);
        #endif
        if (bufferSize <= 0) {
            fprintf(stderr, "A Non-Zero Integer Number has to be input for bufferSize.\n");
            exit(EXIT_FAILURE);
        }
        if (liftTime < 0) {
            fprintf(stderr, "A Non-Zero Integer Number has to be input for liftTime.\n");
            exit(EXIT_FAILURE);
        }
        beginSimulation(bufferSize, liftTime);
    } else {
        fprintf(stderr, "Invalid format entered!\nExpected: lift_sim_A m t\nm = buffer size\nt = time required by each lift\n");
        exit(EXIT_FAILURE);
    }
    return(0);
}

/*******************************************************************************
*   PURPOSE: PThread based implementation of the simulation
*   IMPORTS: int bufferSize, int liftTime
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
#ifdef PTHREAD
void beginSimulation(int bufferSize, int liftTime) {
    int totalMovements = 0, totalRequests = 0;
    FILE* out_sim;
    liftStruct* liftZero, *liftOne, *liftTwo, *liftThree;
    pthread_mutexattr_t canShare;
    pthread_mutex_t liftLock;
    pthread_cond_t full, empty;
    pthread_t LiftR, Lift_1, Lift_2, Lift_3;
    int finishedRead = FALSE;
    queue* buffer = createQueue(bufferSize);
    out_sim = fopen("out_sim", "w");
    if (out_sim == NULL) {
        fprintf(stderr, "Unable to write to a file called \"out_sim\".\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutexattr_init(&canShare);
    pthread_mutexattr_setpshared(&canShare, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&liftLock, &canShare);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    liftZero = initLiftStruct(buffer, liftTime, 0, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftOne = initLiftStruct(buffer, liftTime, 1, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftTwo = initLiftStruct(buffer, liftTime, 2, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftThree = initLiftStruct(buffer, liftTime, 3, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    pthread_create(&Lift_1, NULL, &lift, liftOne);
    pthread_create(&Lift_2, NULL, &lift, liftTwo);
    pthread_create(&Lift_3, NULL, &lift, liftThree);
    pthread_create(&LiftR, NULL, &requestt, liftZero);
    pthread_join(LiftR, NULL);
    pthread_join(Lift_1, NULL);
    pthread_join(Lift_2, NULL);
    pthread_join(Lift_3, NULL);
    totalMovements += *((liftOne->liftReturnVals) + 0);
    totalMovements += *((liftTwo->liftReturnVals) + 0);
    totalMovements += *((liftThree->liftReturnVals) + 0);
    totalRequests += *((liftOne->liftReturnVals) + 1);
    totalRequests += *((liftTwo->liftReturnVals) + 1);
    totalRequests += *((liftThree->liftReturnVals) + 1);
    fprintf(out_sim, "Total number of movements: %d\nTotal number of requests: %d\n", totalMovements, totalRequests);
    freeLiftStruct(liftZero);
    freeLiftStruct(liftOne);
    freeLiftStruct(liftTwo);
    freeLiftStruct(liftThree);
    freeQueue(buffer, free);
    fclose(out_sim);
}
#endif

/*******************************************************************************
*   PURPOSE: PThread based implementation selected when PTHREAD and PROCESS not
*   defined.
*   IMPORTS: int BufferSize, int liftTime
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
#if !defined(PTHREAD) && !defined(PROCESS)
void beginSimulation(int bufferSize, int liftTime) {
    int totalMovements = 0, totalRequests = 0;
    FILE* out_sim;
    liftStruct* liftZero, *liftOne, *liftTwo, *liftThree;
    pthread_mutexattr_t canShare;
    pthread_mutex_t liftLock;
    pthread_cond_t full, empty;
    pthread_t LiftR, Lift_1, Lift_2, Lift_3;
    int finishedRead = FALSE;
    queue* buffer = createQueue(bufferSize);
    printf("The Assumption is made you have selected PThread\nIf you wish to choose Process please compile with Process defined.\n");
    out_sim = fopen("out_sim", "w");
    if (out_sim == NULL) {
        fprintf(stderr, "Unable to write to a file called \"out_sim\".\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutexattr_init(&canShare);
    pthread_mutexattr_setpshared(&canShare, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&liftLock, &canShare);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    liftZero = initLiftStruct(buffer, liftTime, 0, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftOne = initLiftStruct(buffer, liftTime, 1, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftTwo = initLiftStruct(buffer, liftTime, 2, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    liftThree = initLiftStruct(buffer, liftTime, 3, &liftLock, &full,
    &empty, &finishedRead, bufferSize, out_sim);
    pthread_create(&Lift_1, NULL, &lift, liftOne);
    pthread_create(&Lift_2, NULL, &lift, liftTwo);
    pthread_create(&Lift_3, NULL, &lift, liftThree);
    pthread_create(&LiftR, NULL, &requestt, liftZero);
    pthread_join(LiftR, NULL);
    pthread_join(Lift_1, NULL);
    pthread_join(Lift_2, NULL);
    pthread_join(Lift_3, NULL);
    totalMovements += *((liftOne->liftReturnVals) + 0);
    totalMovements += *((liftTwo->liftReturnVals) + 0);
    totalMovements += *((liftThree->liftReturnVals) + 0);
    totalRequests += *((liftOne->liftReturnVals) + 1);
    totalRequests += *((liftTwo->liftReturnVals) + 1);
    totalRequests += *((liftThree->liftReturnVals) + 1);
    fprintf(out_sim, "Total number of movements: %d\nTotal number of requests: %d\n", totalMovements, totalRequests);
    freeLiftStruct(liftZero);
    freeLiftStruct(liftOne);
    freeLiftStruct(liftTwo);
    freeLiftStruct(liftThree);
    freeQueue(buffer, free);
    fclose(out_sim);
}
#endif

/*******************************************************************************
*   PURPOSE: Process implementation for simulation
*   IMPORTS: int bufferSize, int liftTime
*   EXPORTS: none
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
#ifdef PROCESS
void beginSimulation(int bufferSize, int liftTime) {
    /*int totalMovements = 0, totalRequests = 0;*/
    processLift *liftOne, *liftTwo, *liftThree,  *liftZero;
    pid_t mainProcess, LiftR, Lift_1, Lift_2, Lift_3;
    arrayQueue* reqQueue;
    int totalMovements, totalRequests;
    int* liftOneReturns = (int*)mmap(NULL,sizeof(int) * 2,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    int* liftTwoReturns = (int*)mmap(NULL,sizeof(int) * 2,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    int* liftThreeReturns = (int*)mmap(NULL,sizeof(int) * 2,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    request* requestBuffer = (request*)mmap(NULL,sizeof(request) * bufferSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    int* readDone = (int*)mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    FILE** out_sim_file = (FILE**)mmap(NULL,sizeof(FILE*),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    sem_t* fullSem = (sem_t*)mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    sem_t* emptySem = (sem_t*)mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    sem_t* liftZeroSem = (sem_t*)mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    sem_t* requestLiftFileSem = (sem_t*)mmap(NULL,sizeof(sem_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANON,-1,0);
    sem_init(emptySem, 5, bufferSize);
    sem_init(fullSem, 5, 0);
    sem_init(liftZeroSem, 5, 1);
    sem_init(requestLiftFileSem, 5, 0);
    liftOne = createProcessLift(&reqQueue, &readDone, liftTime, 1, bufferSize, &out_sim_file, &fullSem, &emptySem, &liftZeroSem, &requestLiftFileSem, &liftOneReturns);
    liftTwo = createProcessLift(&reqQueue, &readDone, liftTime, 2, bufferSize, &out_sim_file, &fullSem, &emptySem, &liftZeroSem, &requestLiftFileSem, &liftTwoReturns);
    liftThree = createProcessLift(&reqQueue, &readDone, liftTime, 3, bufferSize, &out_sim_file, &fullSem, &emptySem, &liftZeroSem, &requestLiftFileSem, &liftThreeReturns);
    liftZero = createProcessLift(&reqQueue, &readDone, liftTime, 0, bufferSize, &out_sim_file, &fullSem, &emptySem, &liftZeroSem, &requestLiftFileSem, NULL);
    *out_sim_file = fopen("out_sim", "w");
    *readDone = 0;
    #ifdef OUTSIMASSTDOUT
    fclose(*out_sim_file);
    *out_sim_file = stdout;
    #endif
    if (*out_sim_file == NULL) {
        fprintf(stderr, "Unable to write to file called out_sim.\n");
        exit(EXIT_FAILURE);
    }
    reqQueue = createArrayQueue(&requestBuffer, bufferSize);
    mainProcess = getppid();
    if (mainProcess == getppid())
        LiftR = fork();
    if (mainProcess == getppid())
        Lift_1 = fork();
    if (mainProcess == getppid())
        Lift_2 = fork();
    if (mainProcess == getppid())
        Lift_3 = fork();
    if (LiftR == 0) {
        processRequest(&liftZero);
    } else if (Lift_1 == 0) {
        liftProcess(&liftOne);
    } else if (Lift_2 == 0) {
        liftProcess(&liftTwo);
    } else if (Lift_3 == 0) {
        liftProcess(&liftThree);
    } else {
        waitpid(LiftR, NULL, 0);
        waitpid(Lift_1, NULL, 0);
        waitpid(Lift_2, NULL, 0);
        waitpid(Lift_3, NULL, 0);
        totalMovements = 0;
        totalRequests = 0;
        totalMovements += *(liftOneReturns + 0);
        totalMovements += *(liftTwoReturns + 0);
        totalMovements += *(liftThreeReturns + 0);
        totalRequests += *(liftOneReturns + 1);
        totalRequests += *(liftTwoReturns + 1);
        totalRequests += *(liftThreeReturns + 1);
        fprintf(*out_sim_file, "Total number of movements: %d\nTotal number of requests: %d\n", totalMovements, totalRequests);
    }
    cleanupArrayQueue(&reqQueue);
    fclose(*out_sim_file);
    free(liftOne);
    free(liftTwo);
    free(liftThree);
    free(liftZero);
    munmap(requestBuffer, sizeof(request) * bufferSize);
    munmap(readDone, sizeof(int));
    munmap(out_sim_file, sizeof(FILE*));
    munmap(fullSem, sizeof(sem_t));
    munmap(emptySem, sizeof(sem_t));
    munmap(liftZeroSem, sizeof(sem_t));
    munmap(requestLiftFileSem, sizeof(sem_t));
    munmap(liftOneReturns, sizeof(int) * 2);
    munmap(liftTwoReturns, sizeof(int) * 2);
    munmap(liftThreeReturns, sizeof(int) * 2);
    #ifdef DEBUG
    printf("pid of main: %d\n", mainProcess);
    #endif
}
#endif
