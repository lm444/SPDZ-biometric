/* Here will be defined the Iris structure, as well
   as all the related methods */

#include "Iris.h"
#include "Common.h"

// Basically, the Iris constructor from an input file.
Iris* readIris(const char* inputFile) {    
	int fd, ret;

	fd = open(inputFile, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");

	Iris* res = (Iris*) malloc(sizeof(Iris));

	res->size = lseek(fd, 0, SEEK_END); 	// offsets to the end of the file to retrieve the size
	if (VERBOSE) printf("Detected iris of %d bytes.\n", res->size);
	lseek(fd, 0, SEEK_SET); 				// resets the offset to the beginning of the file

	res->iriscode = (int*) malloc(res->size/2*sizeof(int));
	res->mask     = (int*) malloc(res->size/2*sizeof(int));

	// This read will populate the iriscode.
	/* POSSIBLE OPTIMIZATION: One single read on a char* buf, then populate the iriscode
	   That's to avoid single-byte accesses to disk. Also, a bit more readable code. */

	int readBytes=0;
	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		res->iriscode[readBytes] = c - '0';
		readBytes++;
	}

	// Same as before, but for the mask.
	
	readBytes=0;
	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		res->mask[readBytes] = c - '0';
		readBytes++;
	}

	if (VERBOSE) {
		int i;
		printf("iriscode: ");
		for (i=0; i<(res->size/2); i++) printf("%d", res->iriscode[i]);
		printf("\nmask: ");
		for (i=0; i<(res->size/2); i++) printf("%d", res->mask[i]);
		printf("\n");
	}
	return res;
}

void printIris(Iris* iris) {
    int i;
	printf("iriscode: ");
	for (i=0; i<(iris->size/2); i++) printf("%d", iris->iriscode[i]);
	printf("\nmask: ");
	for (i=0; i<(iris->size/2); i++) printf("%d", iris->mask[i]);
	printf("\n");
}

/* At this stage the iris has already been parsed correctly.
   Communication protocol in two phases:
   - first, the size of the iris is sent;
   - then, both the iriscode and the mask are sent.
   Receives will happen in the same order. */

void sendIris(Iris* iris, int to) {
    send(to, &iris->size, sizeof(int), 0);

    send(to, iris->iriscode, iris->size/2*sizeof(int), 0);
    send(to, iris->mask,     iris->size/2*sizeof(int), 0);
}

Iris* recvIris(int from) {
    Iris* res = (Iris*) malloc(sizeof(Iris));
    recv(from, &res->size, sizeof(int), 0);

    res->iriscode = (int*) malloc(res->size/2*sizeof(int));
	res->mask     = (int*) malloc(res->size/2*sizeof(int));

    recv(from, res->iriscode, res->size/2*sizeof(int), 0);
    recv(from, res->mask,     res->size/2*sizeof(int), 0);
    return res;
}