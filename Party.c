#include "Party.h"

Party* party_create(int ID, int MACkey, int peer, TripleArray* tripleArray, RandArray* randArray, OpenValArray* openValArray) {
    Party* res        = (Party*) malloc(sizeof(Party));
    res->ID           = ID;
    res->MACkey       = MACkey;
    res->peer         = peer;
    res->tripleArray  = tripleArray;
    res->randArray    = randArray;
    res->openValArray = openValArray;
    return res;    
}
void party_destroy(Party* party) {
    tripleArray_destroy(party->tripleArray);
    randArray_destroy(party->randArray);
    openValArray_destroy(party->openValArray);
    free(party);
}