#ifndef RANDARRAY_H_
#define RANDARRAY_H_

#define RANDARR_MAX 100000;

typedef struct RandArray {
    int* values;
    int size;
    int nextAvailable;
} RandArray;

RandArray* randArray_create(int size);
void randArray_populate(RandArray* arr, int seed);
int* randArray_consume(RandArray* arr, int numRands);
void randArray_print(RandArray* arr);
void randArray_destroy(RandArray* arr);

#endif