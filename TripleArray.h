#ifndef TRIPLEARRAY_H_
#define TRIPLEARRAY_H_

#define MAX_TRIPLES 5000000

typedef struct Triple {
    int a;
    int b;
    int c;
} Triple;

typedef struct TripleArray {
    Triple* triples;
    int size;
    int freeSpace;
    int nextAvailable;
    int nextFree;
} TripleArray;

TripleArray* tripleArray_create(int size);
void tripleArray_destroy(TripleArray* arr);
Triple* tripleArray_consume(TripleArray* arr, int numTriples);
void tripleArray_print(TripleArray* arr);

int tripleArray_send(TripleArray* arr, int numTriples, int to);
TripleArray* tripleArray_recv(int from);

TripleArray** generateTriples(int numTriples);

#endif