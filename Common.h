/*
 * Common.h
 *
 *  Created on: Mar 8, 2020
 *      Author: lorenzo
 */

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


#define IRIS_CLIENT "sample.txt"
#define IRIS_SERVER "sample.txt"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

typedef struct irisStruct {
	int* irisCode;
	int* mask;
	int size;
} iris;

void handle_error(const char* msg);
void shrinkIrisFile();


#endif /* COMMON_H_ */
