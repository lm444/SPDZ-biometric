#ifndef OPENVALARRAY_H_
#define OPENVALARRAY_H_

typedef struct OpenValArray {
    int* values;
    int size;
    int nextAvailable;
    int nextFree;
} OpenValArray;

OpenValArray* openValArray_create(int size);
void openValArray_insert(OpenValArray* arr, int val);
int* openValArray_consume(OpenValArray* arr, int numVals);
void openValArray_print(OpenValArray* arr);
void openValArray_destroy(OpenValArray* arr);

#endif