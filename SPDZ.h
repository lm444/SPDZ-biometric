#ifndef SPDZ_H_
#define SPDZ_H_

#include "Iris.h"
#include "MultTriple.h"
#include "OpenValArray.h"

void spdz_hammingDist(Iris* iris1, Iris* iris2, MultTripleArray* triples, int self, int other, OpenValArray* openValues);

#endif