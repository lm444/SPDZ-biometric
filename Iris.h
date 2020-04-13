#ifndef IRIS_H_
#define IRIS_H_

// Iris size is here defined as length of the iriscode (which is the same as the mask).
typedef struct IrisStruct {
	int* iriscode;
	int* mask;
	int size;
} Iris;

Iris* readIris(const char* inputFile);
void printIris(Iris* iris);
void destroyIris(Iris* iris);

void sendIris(Iris* iris, int to);
Iris* recvIris(int from);

/*
typedef struct IrisSharesStruct {
	Iris* share0;
	Iris* share1;
} IrisShares;
*/

/* An additional structure might help in readability;
   the following methods would refer to it. */

Iris** genIrisShares(Iris* iris);
void printShares(Iris** shares);
void destroyShares(Iris** shares);

#endif