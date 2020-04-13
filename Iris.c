#include "Iris.h"
#include "Common.h"
#include "Communication.h"

#define IRIS_PRIME 7  // Max random value for the iris feature share generation

Iris* createIris(int size) {
	Iris* res = (Iris*) malloc(sizeof(Iris));

	res->size 	  = size;
	res->iriscode = (int*) malloc(res->size*sizeof(int));
	res->mask     = (int*) malloc(res->size*sizeof(int));

	if (VERBOSE) printf("New iris of size %d at %p.\n", res->size, res);
	return res;
}

Iris* readIris(const char* inputFile) {    
	int fd, ret, size;

	fd = open(inputFile, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");


	size = lseek(fd, 0, SEEK_END)/2; 	/* offsets to the end of the file to retrieve the size.
										   file contains both iriscode and mask; hence the /2. */
	lseek(fd, 0, SEEK_SET); 			// resets the offset to the beginning of the file.

	Iris* res = createIris(size);

	// This read will populate the iriscode.
	/* POSSIBLE OPTIMIZATION: One single read on a char* buf, then populate the iriscode
	   That's to avoid single-byte accesses to disk. Also, a bit more readable code. */
	// TODO: Check for failures on read

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

	if (VERBOSE==2) printIris(res);
	return res;
}

void printIris(Iris* iris) {
    int i;
	printf("Printing iris %p\n", iris);
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

// Will return an array of two Irises containing the respective shares
// Shares will be generated inside the interval (-IRIS_PRIME-1, IRIS_PRIME)

Iris** genIrisShares(Iris* iris) {
	int i;
	Iris** res = malloc(2*sizeof(Iris*));
	res[0] = createIris(iris->size);
	res[1] = createIris(iris->size);

	for (i=0; i<iris->size; i++) {
		res[0]->iriscode[i]=(iris->iriscode[i]-rand()+RAND_MAX/2)%IRIS_PRIME;
		res[1]->iriscode[i]=iris->iriscode[i]-res[0]->iriscode[i];

		res[0]->mask[i]=(iris->mask[i]-rand()+RAND_MAX/2)%IRIS_PRIME;
		res[1]->mask[i]=iris->mask[i]-res[0]->mask[i];
	}

	if (DEBUG||VERBOSE==2) {
		for (i=0; i<iris->size; i++) {
			if (VERBOSE==2) {
				printf("iriscode[%d]=%d; Share0[%d]=%d; Share1[%d]=%d\n", i, iris->iriscode[i], i, res[0]->iriscode[i], i, res[1]->iriscode[i]);
				printf("mask[%d]=%d; Share0[%d]=%d; Share1[%d]=%d\n", i, iris->mask[i], i, res[0]->mask[i], i, res[1]->mask[i]);
			}
			if (DEBUG) {
				assert((res[0]->iriscode[i]+res[1]->iriscode[i])%IRIS_PRIME == iris->iriscode[i]);
				assert((res[0]->mask[i]+res[1]->mask[i])%IRIS_PRIME == iris->mask[i]);
			}
		}
	}
	return res;
}

void printShares(Iris** shares) {
	int i;
	printf("Printing shares %p\n", shares);
	printf("iriscode\n");
	for (i=0; i<shares[0]->size; i++) printf("Share0[%d]=%d; Share1[%d]=%d\n", i, shares[0]->iriscode[i], i, shares[1]->iriscode[i]);
	printf("mask\n");
	for (i=0; i<shares[0]->size; i++) printf("Share0[%d]=%d; Share1[%d]=%d\n", i, shares[0]->mask[i], i, shares[1]->mask[i]);
	printf("\n");
}

void destroyShares(Iris** shares) {
	if (VERBOSE) printf("Freeing shares %p\n", shares);
	destroyIris(shares[0]);
	destroyIris(shares[1]);
	free(shares);
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
	
	if (VERBOSE) printf("Sent iris of size: %d.\n", iris->size);
}

Iris* recvIris(int from) {
	int size;
    recvFrom(from, &size, sizeof(int), 0);

	Iris* res = createIris(size);

    recvFrom(from, res->iriscode, res->size*sizeof(int), 0);
    recvFrom(from, res->mask,     res->size*sizeof(int), 0);

	if (VERBOSE) printf("Received iris of size: %d.\n", res->size);
    return res;
}