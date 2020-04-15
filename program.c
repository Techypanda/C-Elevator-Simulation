#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include "queue.h"
#include "program.h"
#include "request.h"
#include "lifts.h"

int main(int argc, char** argv) {
    char* endptr;
    int bufferSize, liftTime;
    if (argc == 3) {
        bufferSize = strtol(argv[1], &endptr, 10);
        if ((errno == ERANGE && (bufferSize == LONG_MAX || bufferSize == LONG_MIN)) || (errno != 0 && bufferSize == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        if (endptr == argv[1]) {
            fprintf(stderr, "No Digits were found for m.\n");
            exit(EXIT_FAILURE);
        }
        if (*endptr != '\0')
            printf("Further Digits were cut off from m as they couldnt be stored.");
        liftTime = strtol(argv[2], &endptr, 10);
        if (endptr == argv[2]) {
            fprintf(stderr, "No Digits were found for t.\n");
            exit(EXIT_FAILURE);
        }
        if (*endptr != '\0')
            printf("Further Digits were cut off from t as they couldnt be stored.");
        if ((errno == ERANGE && (liftTime == LONG_MAX || liftTime == LONG_MIN)) || (errno != 0 && liftTime == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        #ifdef DEBUG
        printf("Reached with values m:%d and t:%d\n",bufferSize,liftTime);
        #endif
        beginSimulation(bufferSize, liftTime);
    } else {
        fprintf(stderr, "Invalid format entered!\nExpected: lift_sim_A m t\nm = buffer size\nt = time required by each lift\n");
        exit(EXIT_FAILURE);
    }
    return(0);
}

void beginSimulation(int bufferSize, int liftTime) {
    liftStruct* liftZero, *liftOne, *liftTwo, *liftThree;
    pthread_mutexattr_t canShare;
    pthread_mutex_t liftLock;
    pthread_cond_t full, empty;
    pthread_t LiftR, Lift_1, Lift_2, Lift_3;
    int finishedRead = FALSE;
    queue* buffer = createQueue(bufferSize);
    pthread_mutexattr_init(&canShare);
    pthread_mutexattr_setpshared(&canShare, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&liftLock, &canShare);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    liftZero = initLiftStruct(buffer, liftTime, 0, &liftLock, &full,
    &empty, &finishedRead, bufferSize);
    liftOne = initLiftStruct(buffer, liftTime, 1, &liftLock, &full,
    &empty, &finishedRead, bufferSize);
    liftTwo = initLiftStruct(buffer, liftTime, 2, &liftLock, &full,
    &empty, &finishedRead, bufferSize);
    liftThree = initLiftStruct(buffer, liftTime, 3, &liftLock, &full,
    &empty, &finishedRead, bufferSize);
    pthread_create(&Lift_1, NULL, &lift, liftOne);
    pthread_create(&Lift_2, NULL, &lift, liftTwo);
    pthread_create(&Lift_3, NULL, &lift, liftThree);
    pthread_create(&LiftR, NULL, &requestt, liftZero);
    pthread_join(LiftR, NULL);
    pthread_join(Lift_1, NULL);
    pthread_join(Lift_2, NULL);
    pthread_join(Lift_3, NULL);
    freeLiftStruct(liftZero);
    freeLiftStruct(liftOne);
    freeLiftStruct(liftTwo);
    freeLiftStruct(liftThree);
    freeQueue(buffer, free);
}
