#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
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
        printf("Reached with values m:%d and t:%d\n",bufferSize,liftTime);
    } else {
        fprintf(stderr, "Invalid format entered!\nExpected: lift_sim_A m t\nm = buffer size\nt = time required by each lift\n");
        exit(EXIT_FAILURE);
    }
    return(0);    
}
