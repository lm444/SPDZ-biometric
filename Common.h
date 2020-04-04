#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>


#define IRIS_CLIENT "sample.txt"
#define IRIS_SERVER "sample.txt"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define DEALER_ADDR "127.0.0.1"
#define DEALER_PORT 5556

#define VERBOSE 	0
#define DEBUG 		1

// Later will implement the converter in a different executable
#define CONVERTER 	1

typedef struct MultTripleStruct {
    int a;
    int b;
    int c;
} MultTriple;
#define MAX_TRIPLES 128000 


void handle_error(const char* msg);
void shrinkIrisFile();


#endif
