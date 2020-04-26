// There will be defined operations in the clear (aka without SPDZ)
// Utility in checking results.
// Will be a lot verbose 

#include "Common.h"
#include "Iris.h"

void AuthCheckClear(Iris* iris1, Iris* iris2) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
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

        num += (f1+f2-2*f1*f2)*(1-(m1+m2-m1*m2));
        den -= (m1+m2-m1*m2);
    }

    printf("num: %d; den: %d\n", num, den);

    if (num<den*THRESHOLD) printf("Authentication succeded.\n");
    else printf("Authentication failed.\n");
}