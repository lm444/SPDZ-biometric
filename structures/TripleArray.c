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

void tripleArray_populate(TripleArray* arr, int MACkey) {
    int i;

    Triple* triples = arr->triples;

    for (i=0; i<arr->size; i++) {
        triples[i].a = rand()%TRIPLE_MAX_VAL;
        triples[i].b = rand()%TRIPLE_MAX_VAL;
        triples[i].c = triples[i].a*triples[i].b;

        triples[i].MAC_a = MACkey*triples[i].a;
        triples[i].MAC_b = MACkey*triples[i].b;
        triples[i].MAC_c = MACkey*triples[i].c;         
    }
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

TripleArray** tripleArray_genShares(TripleArray* arr) {
    int i;
    int randMAC_a, randMAC_b, randMAC_c;
    int rand_a, rand_b, rand_c;

    TripleArray** res = (TripleArray**) malloc(2*sizeof(Triple*));

    res[SERVER] = tripleArray_create(arr->size);
    res[CLIENT] = tripleArray_create(arr->size);

    // Will compact a bit references.

    Triple* triples      = arr->triples;
    Triple* serverShares = res[SERVER]->triples;
    Triple* clientShares = res[CLIENT]->triples;
    int numTriples       = arr->size;
    
    for (i=0; i<numTriples; i++) {
        rand_a = rand()%TRIPLE_MAX_VAL;
        rand_b = rand()%TRIPLE_MAX_VAL;
        rand_c = rand()%TRIPLE_MAX_VAL;

        randMAC_a = rand()%TRIPLE_MAX_VAL;
        randMAC_b = rand()%TRIPLE_MAX_VAL;
        randMAC_c = rand()%TRIPLE_MAX_VAL;

        serverShares[i].a = triples[i].a - rand_a;
        serverShares[i].b = triples[i].b - rand_b;
        serverShares[i].c = triples[i].c - rand_c;

        clientShares[i].a = rand_a;
        clientShares[i].b = rand_b;
        clientShares[i].c = rand_c;

        serverShares[i].MAC_a = triples[i].MAC_a - randMAC_a;
        serverShares[i].MAC_b = triples[i].MAC_b - randMAC_b;
        serverShares[i].MAC_c = triples[i].MAC_c - randMAC_c;

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
        else if (VERBOSE==2) printTriples=numTriples;
        for (i=0; i<numTriples; i++) {
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
                int shared_MAC_a   = serverShares[i].MAC_a + clientShares[i].MAC_a;
                int shared_MAC_b   = serverShares[i].MAC_b + clientShares[i].MAC_b;
                int shared_MAC_c   = serverShares[i].MAC_c + clientShares[i].MAC_c;
                int expected_MAC_a = triples[i].MAC_a;
                int expected_MAC_b = triples[i].MAC_b;
                int expected_MAC_c = triples[i].MAC_c;
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