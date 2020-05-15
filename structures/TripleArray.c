#include "TripleArray.h"
#include "../Communication.h"
#include "../Common.h"

#define TRIPLE_MAX_VAL 10

TripleArray* tripleArray_create(int size) {
    TripleArray* res    = (TripleArray*) malloc(sizeof(TripleArray));
    res->triples        = (Triple*) malloc(size*sizeof(Triple));
    res->size           = size;
    res->freeSpace      = size;
    res->nextAvailable  = 0;
    res->nextFree       = 0;
    return res;
}

void tripleArray_destroy(TripleArray* arr) {
    free(arr->triples);
    free(arr);
}

// Returns a pointer starting from triples[nextAvailable].
// Updates internal counters accordingly
Triple* tripleArray_consume(TripleArray* arr, int numTriples) {
    Triple* res        = arr->triples+arr->nextAvailable;
    arr->freeSpace     = arr->freeSpace+numTriples;
    arr->nextAvailable = (arr->nextAvailable+numTriples)%arr->size;
    if (VERBOSE && numTriples>1) printf("Consumed %d triple(s). New free space: %d.\n", numTriples, arr->freeSpace);
    else if (VERBOSE==2) printf("Consumed %d triple(s). New free space: %d.\n", numTriples, arr->freeSpace);
    return res;
}

// If VERBOSE==2 will print all the triples
// else will print the first and the last DEBUG_ELEMS elems
void tripleArray_print(TripleArray* arr) {
	int i, max=DEBUG_PRINTELEMS;
	int reverse=arr->size-1;
	if (VERBOSE==2) max = arr->size;
    Triple* triples = arr->triples;

	for (i=0; i<max; i++) {
		printf("Triple[%d] = %d %d %d\n", i, triples[i].a, triples[i].b, triples[i].c);
        printf("TripleMAC[%d] = %d %d %d\n", i, triples[i].MAC_a, triples[i].MAC_b, triples[i].MAC_c);
		if (VERBOSE<2) {
            printf("Triple[%d] = %d %d %d\n", reverse, triples[reverse].a, triples[reverse].b, triples[reverse].c);
            printf("TripleMAC[%d] = %d %d %d\n", reverse, triples[reverse].MAC_a, triples[reverse].MAC_b, triples[reverse].MAC_c);
            reverse--;
        }
	}
}

TripleArray** generateTriples(int numTriples, int MACKey) {
    int i;
    int randA, randB, randC;
    int randMAC_a, randMAC_b, randMAC_c;

    TripleArray** res = (TripleArray**) malloc(2*sizeof(Triple*));

    res[SERVER] = tripleArray_create(numTriples);
    res[CLIENT] = tripleArray_create(numTriples);
    
    // Will compact a bit references.

    Triple* serverShares = res[SERVER]->triples;
    Triple* clientShares = res[CLIENT]->triples;

    for (i=0; i<MAX_TRIPLES; i++) {
        randA = rand()%TRIPLE_MAX_VAL;
        randB = rand()%TRIPLE_MAX_VAL;
        randC = randA*randB;

        randMAC_a = rand()%TRIPLE_MAX_VAL;
        randMAC_b = rand()%TRIPLE_MAX_VAL;
        randMAC_c = rand()%TRIPLE_MAX_VAL;

        serverShares[i].a = randA - rand()%TRIPLE_MAX_VAL;
        serverShares[i].b = randB - rand()%TRIPLE_MAX_VAL;
        serverShares[i].c = randC - rand()%(TRIPLE_MAX_VAL*TRIPLE_MAX_VAL);

        clientShares[i].a = randA - serverShares[i].a;
        clientShares[i].b = randB - serverShares[i].b;
        clientShares[i].c = randC - serverShares[i].c;

        serverShares[i].MAC_a = (serverShares[i].a+clientShares[i].a)*MACKey - randMAC_a;
        serverShares[i].MAC_b = (serverShares[i].b+clientShares[i].b)*MACKey - randMAC_b;
        serverShares[i].MAC_c = (serverShares[i].c+clientShares[i].c)*MACKey - randMAC_c;

        clientShares[i].MAC_a = randMAC_a;
        clientShares[i].MAC_b = randMAC_b;
        clientShares[i].MAC_c = randMAC_c;
    }

    // Generic checks over generation of triples or prints. 
    // The loop will run if either DEBUG or VERBOSE is not zero.
    // Will not check by default since it would weigh down computation.

    if (DEBUG|VERBOSE) {
        int printTriples;
        if (VERBOSE==1) printTriples=DEBUG_PRINTELEMS;
        else if (VERBOSE==2) printTriples=MAX_TRIPLES;
        for (i=0; i<MAX_TRIPLES; i++) {
            if (VERBOSE==1 && i<printTriples) {
                printf("TripleShares[SERVER][%d] = %d, %d, %d", i, serverShares[i].a, serverShares[i].b, serverShares[i].c);
                printf("\n");
                printf("MAC[SERVER][%d] = %d, %d, %d", i, serverShares[i].MAC_a, serverShares[i].MAC_b, serverShares[i].MAC_c);
                printf("\n");
                printf("TripleShares[CLIENT][%d] = %d, %d, %d", i, clientShares[i].a, clientShares[i].b, clientShares[i].c);
                printf("\n");
                printf("MAC[CLIENT][%d] = %d, %d, %d", i, clientShares[i].MAC_a, clientShares[i].MAC_b, clientShares[i].MAC_c);
                printf("\n");

            }
            else if (VERBOSE==2) {
                printf("TripleShares[SERVER][%d] = %d, %d, %d", i, serverShares[i].a, serverShares[i].b, serverShares[i].c);
                printf("\n");
                printf("MAC[SERVER][%d] = %d, %d, %d", i, serverShares[i].MAC_a, serverShares[i].MAC_b, serverShares[i].MAC_c);
                printf("\n");
                printf("TripleShares[CLIENT][%d] = %d, %d, %d", i, clientShares[i].a, clientShares[i].b, clientShares[i].c);
                printf("\n");
                printf("MAC[CLIENT][%d] = %d, %d, %d", i, clientShares[i].MAC_a, clientShares[i].MAC_b, clientShares[i].MAC_c);
                printf("\n");
            }
            if (DEBUG) {
                int shared_a = serverShares[i].a+clientShares[i].a;
                int shared_b = serverShares[i].b+clientShares[i].b;
                int shared_c = serverShares[i].c+clientShares[i].c;
                assert(shared_a*shared_b == shared_c);
                int shared_MAC_a   = serverShares[i].a*MACKey + clientShares[i].a*MACKey;
                int shared_MAC_b   = serverShares[i].b*MACKey + clientShares[i].b*MACKey;
                int shared_MAC_c   = serverShares[i].c*MACKey + clientShares[i].c*MACKey;
                int expected_MAC_a = shared_a*MACKey;
                int expected_MAC_b = shared_b*MACKey;
                int expected_MAC_c = shared_c*MACKey;
                assert(shared_MAC_a == expected_MAC_a);
                assert(shared_MAC_b == expected_MAC_b);
                assert(shared_MAC_c == expected_MAC_c);
            }
        }
    }

    res[SERVER]->freeSpace = res[SERVER]->freeSpace-numTriples;
    res[SERVER]->nextFree  = (res[SERVER]->nextAvailable+numTriples)%res[SERVER]->size;
    res[CLIENT]->freeSpace = res[CLIENT]->freeSpace-numTriples;
    res[CLIENT]->nextFree  = (res[CLIENT]->nextAvailable+numTriples)%res[CLIENT]->size;
    return res;
}

// Will send only the actual triples.
// Other fields will be reconstructed by the recipient.
int tripleArray_send(TripleArray* arr, int numTriples, int to) {
    int ret;
    Triple* triples = tripleArray_consume(arr, numTriples);
    ret = sendTo(to, &numTriples, sizeof(int), 0);
    assert(ret==4);

    ret=sendTo(to, triples, numTriples*sizeof(Triple), 0);
    assert(ret==numTriples*sizeof(Triple));
    return numTriples;
}

TripleArray* tripleArray_recv(int from) {
    int numTriples; 
    recvFrom(from, &numTriples, sizeof(int), 0);
    
    TripleArray* res = tripleArray_create(numTriples);
    recvFrom(from, res->triples, numTriples*sizeof(Triple), 0);
    res->freeSpace = res->freeSpace-numTriples;
    res->nextFree  = (res->nextAvailable+numTriples)%res->size;
    return res;
}