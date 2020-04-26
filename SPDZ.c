#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Iris.h"

// [IMPORTANT] Nothing of this module is used yet in this version. WIP.

// Assumption: Everyone follows the protocol taking triples in order from their list.
// Could guard against it, but complexity would increase (more overhead in the MultTriple struct)

int spdz_mult(int x, int y, MultTriple* triple, int self, int other) {
    int epsilonShare, deltaShare; // known
    int epsilon, delta;           // each party will know them after communication

    epsilonShare = x-triple->a;
    deltaShare   = y-triple->b;
    
    sendIntShare(epsilonShare, other);
    sendIntShare(deltaShare, other);
    epsilon = epsilonShare + recvIntShare(other);
    delta   = deltaShare   + recvIntShare(other);

    if (self==SERVER) 
        return (triple->c + triple->b*epsilon + triple->a*delta + epsilon*delta)%P_FIELD;
    else   
        return (triple->c + triple->b*epsilon + triple->a*delta)%P_FIELD;
}

// This method will consume triples starting from triples
void spdz_hamming_dist(Iris* iris1, Iris* iris2, MultTriple* triples, int self, int other) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
    }

    MultTriple* tracker = triples;

    // Hamming distance
    int i;
    int num=0;
    int den=iris1->size;

    for (i=0; i<iris1->size; i++) {
        int f1 = iris1->iriscode[i];
        int f2 = iris2->iriscode[i];
        int m1 = iris1->mask[i];
        int m2 = iris2->mask[i];

        int f1f2 = (spdz_mult(f1, f2, tracker, self, other));
        int m1m2 = (spdz_mult(m1, m2, tracker, self, other));

        num += (f1+f2-2*f1f2)*(1-(m1+m2-m1m2));
        den -= (m1+m2-m1m2);
        tracker++;
    }

    printf("[SPDZ_HD_%d] num: %d; den: %d\n", self, num, den);
}