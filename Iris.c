/* Here will be defined the Iris structure, as well
   as all the related methods */

#include "Iris.h"
#include "Common.h"

// Basically, the Iris constructor from an input file.
Iris* readIris (const char* inputFile) {    
	int fd, ret;

	fd = open(inputFile, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");

	Iris* res = (Iris*) malloc(sizeof(Iris));

	res->size = lseek(fd, 0, SEEK_END); 	// offsets to the end of the file to check for size consistency
	if (VERBOSE) printf("Detected iris of %d bytes.\n", res->size);
	lseek(fd, 0, SEEK_SET); 				// resets the offset to the beginning of the file

	int* iriscode = (int*) malloc(res->size/2*sizeof(int));
	int* mask     = (int*) malloc(res->size/2*sizeof(int));

	// This read will populate the iriscode.
	/* POSSIBLE OPTIMIZATION: One single read on a char* buf, then populate the iriscode
	   That's to avoid single-byte accesses to disk. Also, a bit more readable code. */

	int readBytes=0;
	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		iriscode[readBytes] = c - '0';
		readBytes++;
	}

	// Same as before, but for the mask.
	
	readBytes=0;
	while (readBytes<(res->size/2)) {
		char c;
		ret = read(fd, &c, 1);
		mask[readBytes] = c - '0';
		readBytes++;
	}

	if (VERBOSE) {
		int i;
		printf("iriscode: ");
		for (i=0; i<(res->size/2); i++) printf("%d", iriscode[i]);
		printf("\nmask: ");
		for (i=0; i<(res->size/2); i++) printf("%d", mask[i]);
		printf("\n");
	}

    res->iriscode=iriscode;
    res->mask=mask;

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