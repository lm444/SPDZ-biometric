#ifndef TRIPLEARRAY_H_
#define TRIPLEARRAY_H_

typedef struct Triple {
    int a;
    int MAC_a;
    int b;
    int MAC_b;
    int c;
    int MAC_c;
} Triple;

typedef struct TripleArray {
    Triple* triples;
    int size;
    int nextAvailable;
} TripleArray;

TripleArray* tripleArray_create(int size);
void tripleArray_destroy(TripleArray* arr);
void tripleArray_populate(TripleArray* arr, int MACkey);
Triple* tripleArray_consume(TripleArray* arr, int numTriples);
void tripleArray_print(TripleArray* arr);

int tripleArray_send(TripleArray* arr, int numTriples, int to);
TripleArray* tripleArray_recv(int from);

TripleArray** tripleArray_genShares(TripleArray* arr);
#endif