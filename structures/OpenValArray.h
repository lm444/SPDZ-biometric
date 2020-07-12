#ifndef OPENVALARRAY_H_
#define OPENVALARRAY_H_

typedef struct OpenValArray {
    int* values;
    int* MAC_values;
    int size;
    int nextAvailable;
    int nextFree;
} OpenValArray;

OpenValArray* openValArray_create(int size);
void openValArray_insert(OpenValArray* arr, int val, int MAC_val);
void openValArray_print(OpenValArray* arr);
void openValArray_destroy(OpenValArray* arr);

#endif