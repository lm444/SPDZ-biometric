#include "Common.h"

#define IRIS1 "iris6400.txt"
#define BUF_SIZE 65536

void handle_error(const char* msg) {
	perror(msg);
	exit(1);
}

void shrinkIrisFile(const char* inputFile, const char* destFile) {
	int fd, ret;
	fd = open(inputFile, O_RDONLY);
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
	outfd = open(destFile, O_WRONLY|O_CREAT, 0640);
	if (outfd<0) handle_error("Error while opening output file");



	while (writtenBytes<validBytes) {
		ret = write(outfd, res+writtenBytes, validBytes-writtenBytes);
		if (ret<0) handle_error("Write");
		writtenBytes+=ret;
	}
	
	if (VERBOSE) printf("Printed %d bytes\n", writtenBytes);

	if (VERBOSE==2) printf("%s\n",buf);
	printf("[INPUT] strlen: %ld; read: %d\n", strlen(buf), readBytes);
	if (VERBOSE==2) printf("%s\n",res);
	printf("[OUTPUT] strlen: %ld; read: %d\n", strlen(res), validBytes);

	printf("Converted %s to %s successfully.\n", inputFile, destFile);

	free(buf);
	free(res);
}
