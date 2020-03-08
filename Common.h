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

typedef struct irisStruct {
	char* irisCode;
	char* mask;
	int size;
} iris;

void handle_error(const char* msg) {
	perror(msg);
	exit(1);
}

#endif /* COMMON_H_ */
