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
} MultTripleArray;

MultTripleArray** generateTriples(int numTriples);
void destroyMultTripleArray(MultTripleArray* arr);

#endif