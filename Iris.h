#ifndef IRIS_H_
#define IRIS_H_

typedef struct IrisStruct {
	int* iriscode;
	int* mask;
	int size;
} Iris;

Iris* readIris (const char* inputFile);
void printIris(Iris* iris);
void sendIris(Iris* iris, int to);
Iris* recvIris(int from);

#endif