#ifndef HAMMINGDIST_H_
#define HAMMINGDIST_H_

typedef struct HammingDistance {
   int num;
   int den;
} HammingDistance;

HammingDistance* hd_create(int num, int den);
void hd_destroy(HammingDistance* hd);

int hd_send(HammingDistance* hd, int to);
HammingDistance* hd_recv(int from);

#endif