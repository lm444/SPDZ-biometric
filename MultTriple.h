#ifndef MULTTRIPLE_H_
#define MULTTRIPLE_H_

#define MAX_TRIPLES 5000000

typedef struct MultTripleStruct {
    int a;
    int b;
    int c;
} MultTriple;

typedef struct MultTripleArrayStruct {
    MultTriple* circularArray;
    int size;
    int freeSpace;
    int nextAvailable;
    int nextFree;
} MultTripleArray;



MultTripleArray* createMultTripleArray(int size);
void destroyMultTripleArray(MultTripleArray* arr);

MultTriple* consumeTriples(MultTripleArray* arr, int numTriples);

MultTripleArray** generateTriples(int numTriples);
void destroyMultTripleArray(MultTripleArray* arr);

#endif