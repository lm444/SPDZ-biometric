#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Iris.h"
#include "Party.h"

// Assumption: Everyone follows the protocol taking triples in order from their list.
// Could guard against it, but complexity would increase

int T; // temporary global variable for debugs

#define SAVE_OPEN 1

// mode: 0         -> will NOT save open values
// mode: SAVE_OPEN -> will save open values
// Will return an int[2] arr. arr[0] -> value, arr[1] -> MAC_value
int* spdz_mult(int x, int y, int MAC_x, int MAC_y, Party* party, int mode) {
    int* res = malloc(2*sizeof(int));
    int epsilonShare, deltaShare; // known
    int epsilon, delta;           // each party will know them after communication

    Triple* currTriple = tripleArray_consume(party->tripleArray, 1);

    int a = currTriple->a;
    int b = currTriple->b;
    int c = currTriple->c;

    int MAC_a = currTriple->MAC_a;
    int MAC_b = currTriple->MAC_b;
    int MAC_c = currTriple->MAC_c;

    epsilonShare = x-a; // gamma(x)_0 - gamma(a)_0
    deltaShare   = y-b; // 
    
    sendInt(epsilonShare, party->peer);
    sendInt(deltaShare, party->peer);
    epsilon = epsilonShare + recvInt(party->peer);
    delta   = deltaShare   + recvInt(party->peer);

    if (mode==SAVE_OPEN) {
        openValArray_insert(party->openValArray, epsilon, MAC_x-MAC_a);
        openValArray_insert(party->openValArray, delta, MAC_y-MAC_b);
    }

    if (T<10) {
        printf("Check input: %d, %d\n", x, y);
        printf("Check tripleShare: %d, %d, %d\n", a, b, c);
        printf("Check epsilon-delta: %d, %d\n", epsilon, delta);
        printf("Check output (without +e-d): %d\n", c + b*epsilon + a*delta);
        T++;
    }

    if (party->ID==SERVER) {
        res[0] = c + b*epsilon + a*delta + epsilon*delta;
        res[1] = MAC_c + MAC_b*epsilon + MAC_a*delta + party->MACkey*epsilon*delta;
    }
    else {  
        res[0] = c + b*epsilon + a*delta;
        res[1] = MAC_c + MAC_b*epsilon + MAC_a*delta + party->MACkey*epsilon*delta;
    }
    return res;
}

// Populate the iris' MAC fields
// NOTE: iris is the share of an iris. 
// Also, it will require communication.
void spdz_genIrisMACShares(Iris* iris, Party* party) {
    int i;
    for (i=0; i<iris->size; i++) {
        int* MAC_iriscode_struct = spdz_mult(iris->iriscode[i], party->MACkey, 0, 0, party, 0);
        int* MAC_mask_struct     = spdz_mult(iris->mask[i], party->MACkey, 0, 0, party, 0);
        iris->MAC_iriscode[i] = MAC_iriscode_struct[0];
        iris->MAC_mask[i]     = MAC_mask_struct[0];
        free(MAC_iriscode_struct);
        free(MAC_mask_struct);
    }
}

void spdz_hammingDist(Iris* iris1, Iris* iris2, Party* party) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return;
    }
    if (VERBOSE) T=0;
    else T=10;

    // Hamming distance
    int i;
    int num=0, den;
    if (party->ID==SERVER) den=iris1->size;
    else den=0;

    for (i=0; i<iris1->size; i++) { 
        int f1 = iris1->iriscode[i];
        int f2 = iris2->iriscode[i];
        int m1 = iris1->mask[i];
        int m2 = iris2->mask[i];
        int MAC_f1 = iris1->MAC_iriscode[i];
        int MAC_f2 = iris2->MAC_iriscode[i];
        int MAC_m1 = iris1->MAC_mask[i];
        int MAC_m2 = iris2->MAC_mask[i];
        // num += (f1+f2-2*f1*f2)*(1-(m1+m2-m1*m2));
        int* f1f2_struct = spdz_mult(f1, f2, MAC_f1, MAC_f2, party, SAVE_OPEN);
        int* m1m2_struct = spdz_mult(m1, m2, MAC_m1, MAC_m2, party, SAVE_OPEN);

        int f1f2 = f1f2_struct[0];
        int m1m2 = m1m2_struct[0];
        int MAC_f1f2 = f1f2_struct[1];
        int MAC_m1m2 = m1m2_struct[1];

        int num1 = f1+f2-2*f1f2;
        int temp = -(m1+m2-m1m2);
        int MAC_num1 = MAC_f1+MAC_f2-2*MAC_f1f2;
        int MAC_temp = -(MAC_m1+MAC_m2-MAC_m1m2);

        int* num2_struct = spdz_mult(num1, temp, MAC_num1, MAC_temp, party, SAVE_OPEN);
        int num2 = num2_struct[0];

        num += num1+num2;
        den -= (m1+m2-m1m2);
        free(num2_struct);
        free(f1f2_struct);
        free(m1m2_struct);
    }

    printf("[SPDZ_HD_%d] num: %d; den: %d\n", party->ID, num, den);
}

// Dealer will check the result by summing the ro_i values.
void spdz_MACCheck(OpenValArray* openValues, RandArray* randValues, int key, int dealer) {
    int i;
    int a=0, gamma_i=0, ro_i;

    int numVals       = openValues->nextFree;
    int* openVals     = openValues->values;
    int* randVals     = randValues->values;
    int* openVals_MAC = openValues->MAC_values;
    
    printf("Batch checking all %d open values.\n", numVals);
    for (i=0; i<numVals; i++) a+=openVals[i]*randVals[i];
    for (i=0; i<numVals; i++) {
        int gamma_aj_i = openVals_MAC[i];
        gamma_i       += gamma_aj_i*randVals[i];
    }
    ro_i = gamma_i - a*key;
    printf("Broadcasting ro (%d).\n", ro_i);
}