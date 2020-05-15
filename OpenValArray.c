#include "Common.h"
#include "OpenValArray.h"

OpenValArray* openValArray_create(int size) {
    OpenValArray* res  = (OpenValArray*) malloc(sizeof(OpenValArray));
    res->values        = (int*) calloc(size,sizeof(int));
    res->MAC_values    = (int*) calloc(size,sizeof(int));
    res->size          = size;
    res->nextAvailable = 0;
    res->nextFree      = 0;
    return res;
}

void openValArray_insert(OpenValArray* arr, int val, int MAC_val) {
    arr->values[arr->nextFree]     = val;
    arr->MAC_values[arr->nextFree] = MAC_val;
    arr->nextFree++;
}

int* openValArray_consume(OpenValArray* arr, int numVals) {
    int* res           = arr->values+arr->nextAvailable;
    arr->nextAvailable = (arr->nextAvailable+numVals)%arr->size;
    if (VERBOSE && numVals>1) printf("Consumed %d open values(s).\n", numVals);
    else if (VERBOSE==2) printf("Consumed %d open values(s).\n", numVals);
    return res;
}

void openValArray_print(OpenValArray* arr) {
    int i, max=DEBUG_PRINTELEMS;
	int reverse=arr->size-1;
	if (VERBOSE==2) max = arr->size;
    int* values     = arr->values;
    int* MAC_values = arr->MAC_values;

	for (i=0; i<max; i++) {
		printf("OpenVal[%d] = %d\n", i, values[i]);
        printf("OpenValMAC[%d] = %d\n", i, MAC_values[i]);
		if (VERBOSE<2) {
            printf("OpenVal[%d] = %d\n", reverse, values[reverse]);
            printf("OpenValMAC[%d] = %d\n", reverse, MAC_values[reverse]);
            reverse--;
        }
	}
}

void openValArray_destroy(OpenValArray* arr) {
    free(arr->values);
    free(arr);
}