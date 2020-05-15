#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Iris.h"

// Assumption: Everyone follows the protocol taking triples in order from their list.
// Could guard against it, but complexity would increase

int T; // temporary global variable for debugs

#define SAVE_OPEN_NO  0
#define SAVE_OPEN_YES 1

// mode: SAVE_OPEN_NO  -> will NOT save open values
// mode: SAVE_OPEN_YES -> will save open values
int spdz_mult(int x, int y, TripleArray* triples, int self, int other, OpenValArray* openValues, int mode) {
    int epsilonShare, deltaShare; // known
    int epsilon, delta;           // each party will know them after communication

    Triple* currTriple = tripleArray_consume(triples, 1);

    int a = currTriple->a;
    int b = currTriple->b;
    int c = currTriple->c;

    epsilonShare = x-a; // gamma(x)_0 - gamma(a)_0
    deltaShare   = y-b; // 
    
    sendInt(epsilonShare, other);
    sendInt(deltaShare, other);
    epsilon = epsilonShare + recvInt(other);
    delta   = deltaShare   + recvInt(other);

    if (mode==SAVE_OPEN_YES) {
        openValArray_insert(openValues, epsilon, 123123123); // temp magic numbers for checks
        openValArray_insert(openValues, delta, 234234234);
    }

    if (T<10) {
        printf("Check input: %d, %d\n", x, y);
        printf("Check tripleShare: %d, %d, %d\n", a, b, c);
        printf("Check epsilon-delta: %d, %d\n", epsilon, delta);
        printf("Check output (without +e-d): %d\n", c + b*epsilon + a*delta);
        T++;
    }

    if (self==SERVER) 
        return c + b*epsilon + a*delta + epsilon*delta;
    else   
        return c + b*epsilon + a*delta;
}

void spdz_hammingDist(Iris* iris1, Iris* iris2, TripleArray* triples, int self, int other, OpenValArray* openValues) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
    }
    if (VERBOSE) T=0;
    else T=10;

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
        // num += (f1+f2-2*f1*f2)*(1-(m1+m2-m1*m2));
        int f1f2 = spdz_mult(f1, f2, triples, self, other, openValues, SAVE_OPEN_YES);
        int m1m2 = spdz_mult(m1, m2, triples, self, other, openValues, SAVE_OPEN_YES);

        int num1 = f1+f2-2*f1f2;
        int temp = -(m1+m2-m1m2);
        int num2 = spdz_mult(num1, temp, triples, self, other, openValues, SAVE_OPEN_YES);

        num += num1+num2;
        den -= (m1+m2-m1m2);
    }

    printf("[SPDZ_HD_%d] num: %d; den: %d\n", self, num, den);
}

// Dealer will check the result by summing the ro_i values.
void spdz_MACCheck(OpenValArray* openValues, RandArray* randValues, int key, int dealer) {
    int i;
    int a=0, gamma_i=0, ro_i;

    int numVals   = openValues->nextFree;
    int* openVals = openValues->values;
    int* randVals = randValues->values;
    
    printf("Batch checking all %d open values.\n", numVals);
    for (i=0; i<numVals; i++) a+=openVals[i]*randVals[i];
    for (i=0; i<numVals; i++) {
        int gamma_aj_i = key*openVals[i];         // TODO: check
        gamma_i       += gamma_aj_i*randVals[i];
    }
    ro_i = gamma_i - a*key;
    printf("Broadcasting ro (%d).\n", ro_i);
}