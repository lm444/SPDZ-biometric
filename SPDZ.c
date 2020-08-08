#include "SPDZ.h"
#include "Common.h"
#include "Communication.h"
#include "Defines.h"
#include "./structures/Iris.h"

int debug_print = DEBUG_PRINTELEMS; // prints up to debug_print spdz_mult(s) info

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

// mode: 0              -> will NOT save open values (ignores MAC_x and MAC_y also)
// mode: MULT_SAVEOPEN  -> will save open values
// Will return an int[2] res. res[0] -> value, res[1] -> MAC_value
int* spdz_mult(Party* party, int x, int y, int MAC_x, int MAC_y, int mode) {
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
    
    net_sendInt(epsilonShare, party->peer);
    net_sendInt(deltaShare, party->peer);
    epsilon = epsilonShare + net_recvInt(party->peer);
    delta   = deltaShare   + net_recvInt(party->peer);

    if (mode==MULT_SAVEOPEN) {
        openValArray_insert(party->openValArray, epsilon, MAC_x-MAC_a);
        openValArray_insert(party->openValArray, delta, MAC_y-MAC_b);
    }

    if (VERBOSE && debug_print<DEBUG_PRINTELEMS) {
        printf("Input: (%d, %d)\n", x, y);
        printf("Triple: (%d, %d, %d)\n", a, b, c);
        printf("Open values (epsilon, delta): (%d, %d)\n", epsilon, delta); // sanity check
        if (party->ID==ID_SERVER) printf("Server output: %d\n", c + b*epsilon + a*delta + epsilon*delta);
        else printf("Client output: %d\n", c + b*epsilon + a*delta);
    }

    // only SERVER adds the constant epsilon*delta
    if (party->ID==ID_SERVER) {
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
// No open values will be saved here 
// (check the documentation for additional info)
void spdz_genIrisMACShares(Party* party, Iris* iris) {
    int i;
    if (VERBOSE) debug_print=0;
    for (i=0; i<iris->size; i++) {
        int* MAC_iriscode_struct = spdz_mult(party, iris->iriscode[i], party->MACkey, 0, 0, 0);
        int* MAC_mask_struct     = spdz_mult(party, iris->mask[i], party->MACkey, 0, 0, 0);
        iris->MAC_iriscode[i] = MAC_iriscode_struct[0];
        iris->MAC_mask[i]     = MAC_mask_struct[0];
        free(MAC_iriscode_struct);
        free(MAC_mask_struct);
        debug_print++;
    }
    if (VERBOSE) printf("\n");
}

HammingDistance* spdz_hd(Party* party, Iris* iris1, Iris* iris2) {
    if (iris1->size!=iris2->size) {
        printf("Mismatching iris sizes. Skipping check.\n");
        return NULL;
    }
    if (VERBOSE) debug_print=0;
    else debug_print=DEBUG_PRINTELEMS;

    // Hamming distance, in SPDZ
    int i;
    int num=0, den;

    // only SERVER the constant N in the denominator
    if (party->ID==ID_SERVER) den=iris1->size;
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
        
        if (VERBOSE) printf("[SPDZ_HD] Calculating F1F2 of feature %d.\n", i);
        int* f1f2_struct = spdz_mult(party, f1, f2, MAC_f1, MAC_f2, MULT_SAVEOPEN);
        if (VERBOSE) printf("[SPDZ_HD] Calculating M1M2 of feature %d.\n", i);
        int* m1m2_struct = spdz_mult(party, m1, m2, MAC_m1, MAC_m2, MULT_SAVEOPEN);

        int f1f2 = f1f2_struct[0];
        int m1m2 = m1m2_struct[0];
        int MAC_f1f2 = f1f2_struct[1];
        int MAC_m1m2 = m1m2_struct[1];

        int num1 = f1+f2-2*f1f2;
        int num2;
        // only SERVER holds the constant 1
        if (party->ID == ID_SERVER) num2 = 1-(m1+m2-m1m2);
        else num2 = -(m1+m2-m1m2);
        int MAC_num1 = MAC_f1+MAC_f2-2*MAC_f1f2;
        int MAC_temp = 1*party->MACkey-(MAC_m1+MAC_m2-MAC_m1m2);

        if (VERBOSE) printf("[SPDZ_HD] NUM1: %d, NUM2: %d\n", num1, num2);
        if (VERBOSE) printf("[SPDZ_HD] Calculating NUM of feature %d.\n", i);

        int* num_struct = spdz_mult(party, num1, num2, MAC_num1, MAC_temp, MULT_SAVEOPEN);
        int num_sum = num_struct[0];

        if (VERBOSE) printf("[SPDZ_HD] NUM: %d\n", num);

        num += num_sum;
        den -= (m1+m2-m1m2);
        free(num_struct);
        free(f1f2_struct);
        free(m1m2_struct);

        debug_print++;
    }

    HammingDistance* res = hd_create(num, den);
    printf("[SPDZ_HD_RES_%d] num: %d; den: %d\n", party->ID, num, den);
    return res;
}

// dealer will check the result by summing the sigma_i values.
// returns the share of the Hamming distance of the peer 
// if no corruption was detected; else will terminate.
HammingDistance* spdz_MACCheck(Party* party, int dealer, HammingDistance* hd) {
    int i;
    int a=0, gamma_i=0, sigma_i;

    // simplyfing references
    int* openVals     = party->openValArray->values;
    int* randVals     = party->randArray->values;
    int* openVals_MAC = party->openValArray->MAC_values;
    int MACkey        = party->MACkey;

    // consumes numVals open values and random values
    // starting from openVals[start], randVals[start]
    int numVals       = party->openValArray->nextFree - party->openValArray->nextAvailable;
    int start         = party->openValArray->nextAvailable;

    printf("Batch checking all %d open values.\n", numVals);
    for (i=start; i<numVals; i++) {
        // calculating a
        a += openVals[i]*randVals[i];

        // calculating gamma_i
        int gamma_aj_i = openVals_MAC[i];
        gamma_i       += gamma_aj_i*randVals[i];
    }
    // updating counters
    party->openValArray->nextAvailable += party->openValArray->nextFree;
    party->randArray->nextAvailable    += party->openValArray->nextFree;

    sigma_i = gamma_i - a*MACkey;

    // Committing both the Hamming distance and the sigma_i
    printf("Committing sigma_i (%d).\n", sigma_i);
    net_sendInt(sigma_i, dealer);
    printf("Committing Hamming distance.\n");
    hd_send(hd, dealer);

    int sigma_other = net_recvInt(dealer);
    printf("Received sigma_other from the peer (%d).\n", sigma_other);

    // locally checking if anyone cheated
    if (sigma_i+sigma_other != 0) {
        printf("Aborting computation: somebody cheated!\n");
        net_sendInt(1, dealer);
        exit(1);
    }
    net_sendInt(0, dealer);

    return hd_recv(dealer);
}