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

#include "Defines.h"

#define IRIS_CLIENT "irisClient.txt"
#define IRIS_SERVER "irisServer.txt"

#define SERVER 0
#define CLIENT 1

void handle_error(const char* msg);
int check_features(int n);

#endif
