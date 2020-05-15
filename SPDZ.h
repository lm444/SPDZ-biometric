#ifndef SPDZ_H_
#define SPDZ_H_

#include "./structures/Iris.h"
#include "./structures/TripleArray.h"
#include "./structures/OpenValArray.h"
#include "./structures/RandArray.h"
#include "Party.h"

void spdz_genIrisMACShares(Iris* iris, Party* party);
void spdz_hammingDist(Iris* iris1, Iris* iris2, Party* party);
void spdz_MACCheck(OpenValArray* openValues, RandArray* randValues, int key, int dealer);

#endif