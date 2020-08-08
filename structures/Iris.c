#include "Iris.h"
#include "../Common.h"
#include "../Communication.h"

Iris* iris_create(int size) {
	Iris* res 	  	  = (Iris*) malloc(sizeof(Iris));

	res->size 	      = size;
	res->iriscode     = (int*) malloc(res->size*sizeof(int));
	res->MAC_iriscode = (int*) calloc(size,sizeof(int));
	res->mask         = (int*) malloc(res->size*sizeof(int));
	res->MAC_mask     = (int*) calloc(size,sizeof(int));

	if (VERBOSE) printf("New iris of size %d at %p.\n", res->size, res);
	return res;
}

void iris_destroy(Iris* iris) {
	if (VERBOSE) printf("Freeing iris %p\n", iris);
	free(iris->iriscode);
	free(iris->mask);
	free(iris->MAC_iriscode);
	free(iris->MAC_mask);
	free(iris);
}

Iris* iris_read(const char* input_file) {    
	int fd, ret, size;

	fd = open(input_file, O_RDONLY);
	if (fd<0) handle_error("Error while opening input file");


	size = lseek(fd, 0, SEEK_END)/2; 	/* offsets to the end of the file to retrieve the size.
										   file contains both iriscode and mask; hence the /2. */
	lseek(fd, 0, SEEK_SET); 			// resets the offset to the beginning of the file.

	Iris* res = iris_create(size);

	// this read will populate the iriscode.
	/* POSSIBLE OPTIMIZATION: One single read on a char* buf, then populate the iriscode
	   That's to avoid single-byte accesses to disk. Also, a bit more readable code. */

	int readBytes=0;
	while (readBytes<(res->size)) {
		char c;
		ret = read(fd, &c, 1);
		if (ret<0 && errno==EINTR) continue;
		if (ret<0) handle_error("[IRIS] Error reading");
		res->iriscode[readBytes] = c - '0';
		readBytes++;
	}

	// same as before, but for the mask.
	
	readBytes=0;
	while (readBytes<(res->size)) {
		char c;
		ret = read(fd, &c, 1);
		if (ret<0 && errno==EINTR) continue;
		if (ret<0) handle_error("[IRIS] Error reading");
		res->mask[readBytes] = c - '0';
		readBytes++;
	}

	if (VERBOSE==2) iris_print(res);
	return res;
}

void iris_print(Iris* iris) {
    int i;
	int maxprint=DEBUG_PRINTELEMS;
	if (VERBOSE==2) maxprint=iris->size;
	printf("Printing iris %p\n", iris);
	for (i=0; i<maxprint; i++) {
		printf("Iriscode[%d] = %d\n", i, iris->iriscode[i]);
		printf("MAC_Iriscode[%d] = %d\n", i, iris->MAC_iriscode[i]);
	}	
	for (i=0; i<maxprint; i++) {
		printf("Irismask[%d] = %d\n", i, iris->mask[i]);
		printf("MAC_Irismask[%d] = %d\n", i, iris->MAC_mask[i]);
	}
	printf("\n");
}

// returns an array of two Irises containing the respective shares
// shares will be generated inside the interval (-MAXVAL_IRISCODE-1, MAXVAL_IRISCODE)

Iris** iris_genShares(Iris* iris) {
	int i;
	Iris** res = malloc(2*sizeof(Iris*));
	res[0] = iris_create(iris->size);
	res[1] = iris_create(iris->size);

	for (i=0; i<iris->size; i++) {
		res[0]->iriscode[i]=(iris->iriscode[i]-rand()+RAND_MAX/2)%MAXVAL_IRISCODE;
		res[1]->iriscode[i]=iris->iriscode[i]-res[0]->iriscode[i];

		res[0]->mask[i]=(iris->mask[i]-rand()+RAND_MAX/2)%MAXVAL_IRISCODE;
		res[1]->mask[i]=iris->mask[i]-res[0]->mask[i];
	}

	if (DEBUG||VERBOSE==2) {
		for (i=0; i<iris->size; i++) {
			if (VERBOSE==2) {
				printf("iriscode[%d]=%d; Share0[%d]=%d; Share1[%d]=%d\n", i, iris->iriscode[i], i, res[0]->iriscode[i], i, res[1]->iriscode[i]);
				printf("mask[%d]=%d; Share0[%d]=%d; Share1[%d]=%d\n", i, iris->mask[i], i, res[0]->mask[i], i, res[1]->mask[i]);
			}
			if (DEBUG) {
				assert((res[0]->iriscode[i]+res[1]->iriscode[i])%MAXVAL_IRISCODE == iris->iriscode[i]);
				assert((res[0]->mask[i]+res[1]->mask[i])%MAXVAL_IRISCODE == iris->mask[i]);
			}
		}
	}
	return res;
}

/* at this stage the iris has already been parsed correctly.
   Communication protocol in two phases:
   - first, the size of the iris is sent;
   - then, both the iriscode and the mask are sent.
   receives will happen in the same order. */

void iris_send(Iris* iris, int to) {
    net_send(to, &iris->size, sizeof(int), 0);

    net_send(to, iris->iriscode, iris->size*sizeof(int), 0);
    net_send(to, iris->mask,     iris->size*sizeof(int), 0);
	
	if (VERBOSE) printf("Sent iris of size: %d.\n", iris->size);
}

Iris* iris_recv(int from) {
	int size;
    net_recv(from, &size, sizeof(int), 0);

	Iris* res = iris_create(size);

    net_recv(from, res->iriscode, res->size*sizeof(int), 0);
    net_recv(from, res->mask,     res->size*sizeof(int), 0);

	if (VERBOSE) printf("Received iris of size: %d.\n", res->size);
    return res;
}