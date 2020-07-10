#include "Common.h"
#include "Debug.h"

// calculates the Hamming distance in the clear
HammingDistance* debug_hd(Iris* iris1, Iris* iris2) {
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

        int num1 = f1+f2-2*f1f2;
        int num2 = 1-(m1+m2-m1m2);

        num += num1*num2;
        den -= (m1+m2-m1m2);
    }

    printf("[DEBUG_HD] num: %d; den: %d\n", num, den);

    HammingDistance* res = hd_create(num, den);
    return res;
}
/*
#define SAMPLES_CNT 5
#define SAMPLE_1 "108_1_6400.txt"
#define SAMPLE_2 "108_2_6400.txt"
#define SAMPLE_3 "108_3_6400.txt"
#define SAMPLE_4 "108_4_6400.txt"
#define SAMPLE_5 "108_5_6400.txt"

void sample_execution() {
    char* sampleifs[SAMPLES_CNT];
    sampleifs[0]=conv_if(SAMPLE_1);
    sampleifs[1]=conv_if(SAMPLE_2);
    sampleifs[2]=conv_if(SAMPLE_3);
    sampleifs[3]=conv_if(SAMPLE_4);
    sampleifs[4]=conv_if(SAMPLE_5);

}
*/