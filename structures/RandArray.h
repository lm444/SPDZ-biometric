#ifndef RANDARRAY_H_
#define RANDARRAY_H_

#include "../Defines.h"

typedef struct RandArray {
    int* values;
    int size;
    int nextAvailable;
} RandArray;

RandArray* randArray_create(int size);
void randArray_populate(RandArray* arr, int seed);
void randArray_print(RandArray* arr);
void randArray_destroy(RandArray* arr);

#endif