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

Iris* iris_read(const char* inputFile);
void iris_print(Iris* iris);
void iris_destroy(Iris* iris);

void iris_send(Iris* iris, int to);
Iris* iris_recv(int from);

/*
typedef struct IrisSharesStruct {
	Iris* shareSelf;
	Iris* shareOther;
} IrisShares;
*/

/* An additional structure might help in readability;
   the following methods would refer to it. */

Iris** genIrisShares(Iris* iris);
void printShares(Iris** shares);
void destroyShares(Iris** shares);

#endif