#ifndef SPDZ_H_
#define SPDZ_H_

#include "Iris.h"

#define MAX_TRIPLES 128

typedef struct MultTripleStruct {
    int a;
    int b;
    int c;
} MultTriple;

void spdz_hamming_dist(Iris* iris1, Iris* iris2, MultTriple* triples, int self, int other);

#endif