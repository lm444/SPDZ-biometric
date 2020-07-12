#include "../Common.h"
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

void openValArray_destroy(OpenValArray* arr) {
    free(arr->values);
    free(arr->MAC_values);
    free(arr);
}

void openValArray_insert(OpenValArray* arr, int val, int MAC_val) {
    if (arr->nextFree >= arr->size) {
        printf("Out of bounds.\n");
        exit(1);
    }
    arr->values[arr->nextFree]     = val;
    arr->MAC_values[arr->nextFree] = MAC_val;
    arr->nextFree++;
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