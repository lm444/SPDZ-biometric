#include "../Common.h"
#include "RandArray.h"

RandArray* randArray_create(int size) {
    RandArray* res     = (RandArray*) malloc(sizeof(RandArray));
    res->values        = (int*) malloc(size*sizeof(int));
    res->size          = size;
    res->nextAvailable = 0;
    return res;
}

void randArray_destroy(RandArray* arr) {
    free(arr->values);
    free(arr);
}

void randArray_populate(RandArray* arr, int seed) {
    int i;
    srand(seed);
    for (i=0; i<arr->size; i++) arr->values[i]=rand()%MAXVAL_RANDARR;
}

void randArray_print(RandArray* arr) {
    int i, max=DEBUG_PRINTELEMS;
	int reverse=arr->size-1;
	if (VERBOSE==2) max = arr->size;
    int* values = arr->values;

	for (i=0; i<max; i++) {
		printf("RandArray[%d] = %d\n", i, values[i]);
		if (VERBOSE<2) {
            printf("RandArray[%d] = %d\n", reverse, values[reverse]);
            reverse--;
        }
	}
}