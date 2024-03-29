#ifndef REQUEST_H
#define REQUEST_H
/*******************************************************************************
*   PURPOSE: Struct for holding details about requests.
*   DATE: 17/04/2020 - 9:20PM
*   AUTHOR: Jonathan Wright
*******************************************************************************/
typedef struct request {
    int requestFloor;
    int destinationFloor;
} request;
request createStackRequest(int inRequest, int inDestination);
request* createRequest(int inRequest, int inDestination);
request* createSharedRequest(int inRequest, int inDestination);
void* requestt(void* args);
void processRequest(void* args);
#endif
