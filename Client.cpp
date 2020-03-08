/*
 * main.cpp
 *
 *  Created on: Mar 7, 2020
 *      Author: lorenzo
 */
#include "Common.h"

#define IRIS1 "iris6400.txt"

#define BUF_SIZE 65536
//#define HEADER_SIZE 128


void shrinkIrisFile() {
	int fd, ret;
	fd = open(IRIS1, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");




	char* buf = (char*) malloc(BUF_SIZE*sizeof(char));
	char* res = (char*) malloc((BUF_SIZE/2)*sizeof(char));
//	char* header = (char*) calloc(1, HEADER_SIZE*sizeof(char));

	int readBytes=0, validBytes=0;

	while (1) {
		ret = read(fd, buf+readBytes, 1);
		if (ret<0) handle_error("Read");
		if (ret==0) break;
		if (buf[readBytes]=='0' || buf[readBytes]=='1') {
			res[validBytes]=buf[readBytes];
			validBytes++;
		}
		readBytes++;
	}

	buf[readBytes]='\0';
	res[validBytes]='\0';

	int writtenBytes=0;

/*	sprintf(header, "%d", validBytes);

	while (writtenBytes<HEADER_SIZE) {
		ret = write(outfd, header+writtenBytes, HEADER_SIZE-writtenBytes);
		if (ret<0) handle_error("Write");
		writtenBytes+=ret;
	}
*/

	int outfd;
	outfd = open("sample.txt", O_WRONLY|O_CREAT, 0640);
	if (outfd<0) handle_error("Error while opening output file");



	while (writtenBytes<validBytes) {
		ret = write(outfd, res+writtenBytes, validBytes-writtenBytes);
		if (ret<0) handle_error("Write");
		writtenBytes+=ret;
	}
	printf("printed %d bytes\n", writtenBytes);


	printf("%s\n",buf);
	printf("[GENERAL] strlen: %d; read: %d\n",strlen(buf), readBytes);
	printf("%s\n",res);
	printf("[VALID] strlen: %d; read: %d\n", strlen(res), validBytes);

	free(buf);
	free(res);
}

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
	//printf("SPDZ");


	shrinkIrisFile();
	readIris("sample.txt");
	int i;
	return 0;
}
