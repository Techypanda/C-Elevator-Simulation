#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    char* endptr;
    int bufferSize, liftTime;
    if (argc != 3) {
        bufferSize = strtol(argv[1], &endptr);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
            fprintf(stderr, "Invalid Number Entered for m: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        if (endptr == argv[1]) {
            fprintf(stderr, "No Digits were found for m.\n");
            exit(EXIT_FAILURE);
        }
        if (*endptr != '\0')
            printf("Further Digits were cut off frorm m as they couldnt be stored.");
        liftTime = strtol(argv[2], &endptr);
        if (endptr == argv[1]) {
            fprintf(stderr, "No Digits were found for t.\n");
            exit(EXIT_FAILURE);
        }
        if (*endptr != '\0')
            printf("Further Digits were cut off frorm m as they couldnt be stored.");
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
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
