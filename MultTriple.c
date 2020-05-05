#include "MultTriple.h"
#include "Communication.h"
#include "Common.h"

#define TRIPLE_MAX_VAL 10

MultTripleArray* tripleArray_create(int size) {
    MultTripleArray* res = (MultTripleArray*) malloc(sizeof(MultTripleArray));
    res->triples         = (MultTriple*) malloc(size*sizeof(MultTriple));
    res->size            = size;
    res->freeSpace       = size;
    res->nextAvailable   = 0;
    res->nextFree        = 0;
    return res;
}

void tripleArray_destroy(MultTripleArray* arr) {
    free(arr->triples);
    free(arr);
}

// Returns a pointer starting from triples[nextAvailable].
// Updates internal counters accordingly
MultTriple* tripleArray_consume(MultTripleArray* arr, int numTriples) {
    MultTriple* res    = arr->triples+arr->nextAvailable;
    arr->freeSpace     = arr->freeSpace+numTriples;
    arr->nextAvailable = (arr->nextAvailable+numTriples)%arr->size;
    if (VERBOSE && numTriples>1) printf("Consumed %d triple(s). New free space: %d.\n", numTriples, arr->freeSpace);
    else if (VERBOSE==2) printf("Consumed %d triple(s). New free space: %d.\n", numTriples, arr->freeSpace);
    return res;
}

// If VERBOSE==2 will print all the triples
// else will print the first and the last DEBUG_ELEMS elems
void tripleArray_print(MultTripleArray* arr) {
	int i, max=DEBUG_PRINTELEMS;
	int reverse=arr->size-1;
	if (VERBOSE==2) max = arr->size;
    MultTriple* triples = arr->triples;

	for (i=0; i<max; i++) {
		printf("Triple[%d] = %d %d %d\n", i, triples[i].a, triples[i].b, triples[i].c);
		if (VERBOSE<2) {
            printf("Triple[%d] = %d %d %d\n", reverse, triples[reverse].a, triples[reverse].b, triples[reverse].c);
            reverse--;
        }
	}
}

MultTripleArray** generateTriples(int numTriples) {
    int i;
    int randA, randB, randC;

    MultTripleArray** res = (MultTripleArray**) malloc(2*sizeof(MultTriple*));

    res[SERVER] = tripleArray_create(numTriples);
    res[CLIENT] = tripleArray_create(numTriples);
    
    // Will compact a bit references.

    MultTriple* serverShares = res[SERVER]->triples;
    MultTriple* clientShares = res[CLIENT]->triples;

    for (i=0; i<MAX_TRIPLES; i++) {
        randA = rand()%TRIPLE_MAX_VAL;
        randB = rand()%TRIPLE_MAX_VAL;
        randC = randA*randB;

        serverShares[i].a = randA-rand()%TRIPLE_MAX_VAL;
        serverShares[i].b = randB-rand()%TRIPLE_MAX_VAL;
        serverShares[i].c = randC-rand()%(TRIPLE_MAX_VAL*TRIPLE_MAX_VAL);

        clientShares[i].a = randA-serverShares[i].a;
        clientShares[i].b = randB-serverShares[i].b;
        clientShares[i].c = randC-serverShares[i].c;
    }

    // Generic checks over generation of triples or prints. 
    // The loop will run if either DEBUG or VERBOSE is not zero.
    // Will not check by default since it would weigh down computation.

    if (DEBUG|VERBOSE) {
        for (i=0; i<MAX_TRIPLES; i++) {
            if (VERBOSE==2) {
                printf("MultTripleShares[SERVER][%d] = %d, %d, %d", i, serverShares[i].a, serverShares[i].b, serverShares[i].c);
                printf("\n");
                printf("MultTripleShares[CLIENT][%d] = %d, %d, %d", i, clientShares[i].a, clientShares[i].b, clientShares[i].c);
            }
            if (DEBUG) assert((serverShares[i].a+clientShares[i].a)*(serverShares[i].b+clientShares[i].b)==serverShares[i].c+clientShares[i].c);
            if (VERBOSE==2) printf("\n");
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
int tripleArray_send(MultTripleArray* arr, int numTriples, int to) {
    int ret;
    MultTriple* triples = tripleArray_consume(arr, numTriples);
    ret = sendTo(to, &numTriples, sizeof(int), 0);
    assert(ret==4);

    ret=sendTo(to, triples, numTriples*sizeof(MultTriple), 0);
    assert(ret==numTriples*sizeof(MultTriple));
    return numTriples;
}

MultTripleArray* tripleArray_recv(int from) {
    int numTriples; 
    recvFrom(from, &numTriples, sizeof(int), 0);
    
    MultTripleArray* res = tripleArray_create(numTriples);
    recvFrom(from, res->triples, numTriples*sizeof(MultTriple), 0);
    res->freeSpace = res->freeSpace-numTriples;
    res->nextFree  = (res->nextAvailable+numTriples)%res->size;
    return res;
}