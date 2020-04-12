/* Here will be defined the Iris structure, as well
   as all the related methods */

#include "Iris.h"
#include "Common.h"
#include "Communication.h"

// Basically, the Iris constructor from an input file.
Iris* readIris(const char* inputFile) {    
	int fd, ret;

	fd = open(inputFile, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");

	Iris* res = (Iris*) malloc(sizeof(Iris));

	res->size = lseek(fd, 0, SEEK_END)/2; 	// offsets to the end of the file to retrieve the size.
											// file contains both iriscode and mask; hence the /2.
	if (VERBOSE) printf("Detected iris of %d bytes.\n", res->size);
	lseek(fd, 0, SEEK_SET); 				// resets the offset to the beginning of the file.

	res->iriscode = (int*) malloc(res->size*sizeof(int));
	res->mask     = (int*) malloc(res->size*sizeof(int));

	// This read will populate the iriscode.
	/* POSSIBLE OPTIMIZATION: One single read on a char* buf, then populate the iriscode
	   That's to avoid single-byte accesses to disk. Also, a bit more readable code. */

	int readBytes=0;
	while (readBytes<(res->size)) {
		char c;
		ret = read(fd, &c, 1);
		res->iriscode[readBytes] = c - '0';
		readBytes++;
	}

	// Same as before, but for the mask.
	
	readBytes=0;
	while (readBytes<(res->size)) {
		char c;
		ret = read(fd, &c, 1);
		res->mask[readBytes] = c - '0';
		readBytes++;
	}

	if (VERBOSE) {
		int i;
		printf("iriscode: ");
		for (i=0; i<(res->size); i++) printf("%d", res->iriscode[i]);
		printf("\nmask: ");
		for (i=0; i<(res->size); i++) printf("%d", res->mask[i]);
		printf("\n");
	}
	return res;
}

void printIris(Iris* iris) {
    int i;
	printf("iriscode: ");
	for (i=0; i<iris->size; i++) printf("%d", iris->iriscode[i]);
	printf("\nmask: ");
	for (i=0; i<iris->size; i++) printf("%d", iris->mask[i]);
	printf("\n");
}

void destroyIris(Iris* iris) {
	if (VERBOSE) printf("Freeing iris %p\n", iris);
	free(iris->iriscode);
	free(iris->mask);
	free(iris);
}

/* At this stage the iris has already been parsed correctly.
   Communication protocol in two phases:
   - first, the size of the iris is sent;
   - then, both the iriscode and the mask are sent.
   Receives will happen in the same order. */

void sendIris(Iris* iris, int to) {
    sendTo(to, &iris->size, sizeof(int), 0);

    sendTo(to, iris->iriscode, iris->size*sizeof(int), 0);
    sendTo(to, iris->mask,     iris->size*sizeof(int), 0);
}

Iris* recvIris(int from) {
    Iris* res = (Iris*) malloc(sizeof(Iris));
    recvFrom(from, &res->size, sizeof(int), 0);

    res->iriscode = (int*) malloc(res->size*sizeof(int));
	res->mask     = (int*) malloc(res->size*sizeof(int));

    recvFrom(from, res->iriscode, res->size*sizeof(int), 0);
    recvFrom(from, res->mask,     res->size*sizeof(int), 0);
    return res;
}