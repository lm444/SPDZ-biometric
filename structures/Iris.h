#ifndef IRIS_H_
#define IRIS_H_

// Iris size is here defined as length of the iriscode (which is the same as the mask).
typedef struct Iris {
	int* iriscode;
	int* MAC_iriscode;
	int* mask;
	int* MAC_mask;
	int size;
} Iris;

Iris* iris_read(const char* input_file);
void iris_print(Iris* iris);
void iris_destroy(Iris* iris);

void iris_send(Iris* iris, int to);
Iris* iris_recv(int from);

Iris** iris_genShares(Iris* iris);

#endif