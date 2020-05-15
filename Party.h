#ifndef PARTY_H_
#define PARTY_H_

#include "./structures/OpenValArray.h"
#include "./structures/RandArray.h"
#include "./structures/TripleArray.h"
#include "Common.h"

typedef struct Party {
    int ID;
    int MACkey;
    int peer;
    TripleArray* tripleArray;
    RandArray* randArray;
    OpenValArray* openValArray;
} Party;

Party* party_create(int ID, int MACKey, int peer, TripleArray* tripleArray, RandArray* randArray, OpenValArray* openValArray);
void party_destroy(Party* party);
#endif