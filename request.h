#ifndef REQUEST_H
#define REQUEST_H
typedef struct request {
    int requestFloor;
    int destinationFloor;
} request;
request* createRequest(int inRequest, int inDestination);
request* createSharedRequest(int inRequest, int inDestination);
void* requestt(void* args);
void processRequest(void* args);
#endif
