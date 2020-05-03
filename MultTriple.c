#include "MultTriple.h"
#include "Common.h"

#define TRIPLE_MAX_VAL 10

MultTripleArray* createMultTripleArray(int size) {
    MultTripleArray* res = (MultTripleArray*) malloc(sizeof(MultTripleArray));
    res->triples         = (MultTriple*) malloc(size*sizeof(MultTriple));
    res->size            = size;
    res->freeSpace       = size;
    res->nextAvailable   = 0;
    res->nextFree        = 0;
    return res;
}

void destroyMultTripleArray(MultTripleArray* arr) {
    free(arr->triples);
    free(arr);
}

// Returns a pointer starting from circularArray[nextAvailable].
// Updates internal counters accordingly
MultTriple* consumeTriples(MultTripleArray* arr, int numTriples) {
    MultTriple* res    = arr->triples+arr->nextAvailable;
    arr->freeSpace     = arr->freeSpace+numTriples;
    arr->nextAvailable = (arr->nextAvailable+numTriples)%arr->size;
    if (VERBOSE==2) printf("Consumed %d triple(s). New free space: %d.\n", numTriples, arr->freeSpace);
    return res;
} 

MultTripleArray** generateTriples(int numTriples) {
    int i;
    int randA, randB, randC;

    MultTripleArray** res = (MultTripleArray**) malloc(2*sizeof(MultTriple*));

    res[SERVER] = createMultTripleArray(numTriples);
    res[CLIENT] = createMultTripleArray(numTriples);
    
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