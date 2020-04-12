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
void sendIris(Iris* iris, int to);
Iris* recvIris(int from);
void destroyIris(Iris* iris);

#endif