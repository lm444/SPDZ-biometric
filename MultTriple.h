#ifndef MULTTRIPLE_H_
#define MULTTRIPLE_H_

#define MAX_TRIPLES 5000000

typedef struct MultTriple {
    int a;
    int b;
    int c;
} MultTriple;

typedef struct MultTripleArray {
    MultTriple* triples;
    int size;
    int freeSpace;
    int nextAvailable;
    int nextFree;
} MultTripleArray;



MultTripleArray* tripleArray_create(int size);
void tripleArray_destroy(MultTripleArray* arr);
MultTriple* tripleArray_consume(MultTripleArray* arr, int numTriples);

int tripleArray_send(MultTripleArray* arr, int numTriples, int to);
MultTripleArray* tripleArray_recv(int from);

MultTripleArray** generateTriples(int numTriples);



#endif