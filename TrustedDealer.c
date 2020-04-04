#include "Common.h"

/* Every share will be structured in an array of two fields
   arr[0] will be related to the Server
   arr[1] will be related to the Client */

#define DEBUG_RAND_MAX 100000

int MACshares[2];
MultTriple* MultTripleShares[2];

void generateTriples() {
    int i;
    int randA, randB, randC;

    MultTripleShares[0] = (MultTriple*) malloc(sizeof(MultTriple)*MAX_TRIPLES);
    MultTripleShares[1] = (MultTriple*) malloc(sizeof(MultTriple)*MAX_TRIPLES);
    
    // Will compact a bit references.

    MultTriple* serverShares = MultTripleShares[0];
    MultTriple* clientShares = MultTripleShares[1];

    for (i=0; i<MAX_TRIPLES; i++) {
        randA = rand()/DEBUG_RAND_MAX;
        randB = rand()/DEBUG_RAND_MAX;
        randC = randA*randB;

        serverShares[i].a = randA-rand()/DEBUG_RAND_MAX;
        serverShares[i].b = randB-rand()/DEBUG_RAND_MAX;
        serverShares[i].c = randC-rand()/DEBUG_RAND_MAX;

        clientShares[i].a = randA-serverShares[i].a;
        clientShares[i].b = randB-serverShares[i].b;
        clientShares[i].c = randC-serverShares[i].c;
    }

    if (DEBUG) {
        for (i=0; i<MAX_TRIPLES; i++) {
            printf("[DEBUG] MultTripleShares[%d][%d] = %d, %d, %d", 0, i, serverShares[i].a, serverShares[i].b, serverShares[i].c);
            printf("\n");
            printf("[DEBUG] MultTripleShares[%d][%d] = %d, %d, %d", 1, i, clientShares[i].a, clientShares[i].b, clientShares[i].c);
            assert((serverShares[i].a+clientShares[i].a)*(serverShares[i].b+clientShares[i].b)==serverShares[i].c+clientShares[i].c);
            printf("\n");
        }
    }
}

int main() {
    printf("[TRUSTED DEALER] Now generating MAC and its shares...\n");
    int MAC = rand();
    MACshares[0] = MAC-rand();
    MACshares[1] = MAC-MACshares[0];
    assert(MACshares[0]+MACshares[1]==MAC);
    printf("[TRUSTED DEALER] MAC shares generated.\n");
    printf("[TRUSTED DEALER] Will now generate %d multiplicative triples...\n", MAX_TRIPLES);
    generateTriples();
}