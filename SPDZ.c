#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Iris.h"

// Assumption: Everyone follows the protocol taking triples in order from their list.
// Could guard against it, but complexity would increase

int T=0; // temporary global variable for debugs
int spdz_mult(int x, int y, MultTripleArray* triples, int self, int other, OpenValArray* openValues) {
    int epsilonShare, deltaShare; // known
    int epsilon, delta;           // each party will know them after communication

    MultTriple* currTriple = tripleArray_consume(triples, 1);

    int a = currTriple->a;
    int b = currTriple->b;
    int c = currTriple->c;

    epsilonShare = x-a;
    deltaShare   = y-b;
    
    sendInt(epsilonShare, other);
    sendInt(deltaShare, other);
    epsilon = epsilonShare + recvInt(other);
    delta   = deltaShare   + recvInt(other);

    openValArray_insert(openValues, epsilon);
    openValArray_insert(openValues, delta);

    if (T<10) {
        printf("Check input: %d, %d\n", x, y);
        printf("Check tripleShare: %d, %d, %d\n", a, b, c);
        printf("Check epsilon-delta: %d, %d\n", epsilon, delta);
        printf("Check output (without +e-d): %d\n", c + b*epsilon + a*delta);
    }

    if (self==SERVER) 
        return c + b*epsilon + a*delta + epsilon*delta;
    else   
        return c + b*epsilon + a*delta;
}

void spdz_hammingDist(Iris* iris1, Iris* iris2, MultTripleArray* triples, int self, int other, OpenValArray* openValues) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
    }

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
        int f1f2 = (spdz_mult(f1, f2, triples, self, other, openValues));
        int m1m2 = (spdz_mult(m1, m2, triples, self, other, openValues));

        int num1 = f1+f2-2*f1f2;
        int temp = -(m1+m2-m1m2);
        int num2 = spdz_mult(num1, temp, triples, self, other, openValues);

        num += num1+num2;
        den -= (m1+m2-m1m2);
        T++;
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