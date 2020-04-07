#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    int bufferSize, liftTime;
    if (argc != 3) {
        bufferSize = strtol(argv[1]);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %d\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        liftTime = strtol(argv[2]);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %d\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        printf("Reached with values m:%d and t:%d\n",bufferSize,liftTime);
    } else {
        fprintf(stderr, "Invalid format entered!\nExpected: lift_sim_A m t\nm = buffer size\nt = time required by each lift\n");
        exit(EXIT_FAILURE);
    }
    return(0);    
}
