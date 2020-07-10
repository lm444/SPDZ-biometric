#ifndef SPDZ_H_
#define SPDZ_H_

#include "./structures/Iris.h"
#include "./structures/TripleArray.h"
#include "./structures/OpenValArray.h"
#include "./structures/RandArray.h"
#include "./structures/HammingDist.h"

typedef struct Party {
    int ID;
    int MACkey;
    int peer;
    TripleArray* tripleArray;
    RandArray* randArray;
    OpenValArray* openValArray;
} Party;

Party* party_create(int ID, int MACkey, int peer, TripleArray* tripleArray, RandArray* randArray, OpenValArray* openValArray);
void party_destroy(Party* party);

void spdz_genIrisMACShares(Party* party, Iris* iris);
HammingDistance* spdz_hd(Party* party, Iris* iris1, Iris* iris2);
HammingDistance* spdz_MACCheck(Party* party, int dealer, HammingDistance* hd);

#endif