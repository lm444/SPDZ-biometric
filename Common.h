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

#define SERVER 0
#define CLIENT 1

#define THRESHOLD 0.25
#define P_FIELD 2



/* Different levels of VERBOSE:
   1 - Just some minor additional print
   2 - Includes lenghty prints (care!)
*/
#define VERBOSE 	1
#define DEBUG 		0

#define PRINT_ELEMS 100

// Later will implement the converter in a different executable
#define CONVERTER 1

void handle_error(const char* msg);
void shrinkIrisFile();

#endif
