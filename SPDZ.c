#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Iris.h"


// Assumption: Everyone follows the protocol taking triples in order from their list.
// Could guard against it, but complexity would increase (more overhead in the MultTriple struct)

int T=0; // temporary global variable for debugs
int spdz_mult(int x, int y, MultTriple* triple, int self, int other) {
    int epsilonShare, deltaShare; // known
    int epsilon, delta;           // each party will know them after communication

    epsilonShare = x-triple->a;
    deltaShare   = y-triple->b;
    
    sendIntShare(epsilonShare, other);
    sendIntShare(deltaShare, other);
    epsilon = epsilonShare + recvIntShare(other);
    delta   = deltaShare   + recvIntShare(other);

    if (T<10) {
        printf("Check input: %d, %d\n", x, y);
        printf("Check tripleShare: %d, %d, %d\n", triple->a, triple->b, triple->c);
        printf("Check epsilon-delta: %d, %d\n", epsilon, delta);
        printf("Check output (without +e-d): %d\n", triple->c + triple->b*epsilon + triple->a*delta);
    }

    if (self==SERVER) 
        return triple->c + triple->b*epsilon + triple->a*delta + epsilon*delta;
    else   
        return triple->c + triple->b*epsilon + triple->a*delta;
}

// This method will consume triples starting from triples
void spdz_hamming_dist(Iris* iris1, Iris* iris2, MultTriple* triples, int self, int other) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
    }

    MultTriple* tracker = triples;

    printf("Reference tripleShare: %d, %d, %d\n", tracker->a, tracker->b, tracker->c);

    // Hamming distance
    int i;
    int num=0, den;
    if (self==SERVER) den=iris1->size;
    else den=0;

    for (i=0; i<iris1->size; i++) {
        int f1 = iris1->iriscode[i];
        int f2 = iris2->iriscode[i];
        int m1 = iris1->mask[i];
        int m2 = iris2->mask[i];

        int f1f2 = (spdz_mult(f1, f2, tracker, self, other));
        int m1m2 = (spdz_mult(m1, m2, tracker, self, other));

        int num1 = f1+f2-2*f1f2;
        int num2 = 1-(m1+m2-m1m2);

        num += spdz_mult(num1, num2, tracker, self, other);
        den -= (m1+m2-m1m2);
        T++;
    }

    printf("[SPDZ_HD_%d] num: %d; den: %d\n", self, num, den);
}