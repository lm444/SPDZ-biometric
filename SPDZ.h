#ifndef SPDZ_H_
#define SPDZ_H_

#include "Iris.h"
#include "TripleArray.h"
#include "OpenValArray.h"
#include "RandArray.h"

void spdz_hammingDist(Iris* iris1, Iris* iris2, TripleArray* triples, int self, int other, OpenValArray* openValues);
void spdz_MACCheck(OpenValArray* openValues, RandArray* randValues, int key, int dealer);

#endif