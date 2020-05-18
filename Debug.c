// There will be defined operations in the clear (aka without SPDZ)
// Utility in checking results.
// Will be a lot verbose 

#include "Common.h"
#include "Debug.h"

HammingDistance* debug_hammingDistClear(Iris* iris1, Iris* iris2) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return NULL;
    }

    // Hamming distance, in the clear
    int i;
    int num=0;
    int den=iris1->size;

    for (i=0; i<iris1->size; i++) {
        int f1 = iris1->iriscode[i];
        int f2 = iris2->iriscode[i];
        int m1 = iris1->mask[i];
        int m2 = iris2->mask[i];

        int f1f2 = f1*f2;
        int m1m2 = m1*m2;
/*
        int num1 = f1+f2-2*f1f2;
        int temp = -(m1+m2-m1m2);
        int num2 = num1*temp;

        num += num1+num2;
*/
        int num1 = f1+f2-2*f1f2;
        int num2 = 1-(m1+m2-m1m2);

        num += num1*num2;
        den -= (m1+m2-m1m2);
    }

    printf("[DEBUG_HD] num: %d; den: %d\n", num, den);

    HammingDistance* res = hd_create(num, den);
    return res;
}