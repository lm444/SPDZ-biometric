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


#define IRIS_CLIENT "sample.txt"
#define IRIS_SERVER "sample.txt"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define OUTPUT_BIN  "./bin"

#define VERBOSE 	0

// Later will implement the converter in a different executable
#define CONVERTER 	1

void handle_error(const char* msg);
void shrinkIrisFile();


#endif
