/*
 * main.cpp
 *
 *  Created on: Mar 7, 2020
 *      Author: lorenzo
 */
#include "Common.h"

iris* readIris (const char* inputFile) {
	int fd, ret;

	fd = open(inputFile, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");

	iris* res = (iris*) malloc(sizeof(iris));

	res->size = lseek(fd, 0, SEEK_END); // offsets to the end of the file to check for size consistency
	printf("Detected iris of %d bytes.\n", res->size);
	lseek(fd, 0, SEEK_SET); // clears the offset to the beginning of the file

	int* irisCode = (int*) calloc(1, res->size*sizeof(int));
	int* irisMask = (int*) calloc(1, res->size*sizeof(int));

	int readBytes=0;
	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		irisCode[readBytes] = c - '0';
		readBytes++;
	}

	readBytes=0;

	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		irisMask[readBytes] = c - '0';
		readBytes++;
	}

	int i;
	printf("Iris: ");
	for (i=0; i<(res->size/2); i++) printf("%d", irisCode[i]);
	printf("\nMask: ");
	for (i=0; i<(res->size/2); i++) printf("%d", irisMask[i]);
	printf("\n");

	return res;
}

void placeholder (iris* test) {}


int main(int argc, char** argv) {
	int ret, server_desc;
	struct sockaddr_in server_addr = {0};

	server_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (server_desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	ret = connect(server_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Connect");

	printf("Connection successful.\n");

	shrinkIrisFile();
	readIris(IRIS_CLIENT);
	int i;
	return 0;
}
