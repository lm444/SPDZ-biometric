#include "HammingDist.h"
#include "../Communication.h"
#include "../Common.h"

HammingDistance* hd_create(int num, int den) {
	HammingDistance* res = (HammingDistance*) malloc(sizeof(HammingDistance));
	res->num			 = num;
	res->den		     = den;
	return res;
}

void hd_destroy(HammingDistance* hd) {
	free(hd);
}

int hd_send(HammingDistance* hd, int to) {
    int ret;
    ret=net_send(to, hd, sizeof(HammingDistance), 0);
    assert(ret==sizeof(HammingDistance));
    return ret;
}

HammingDistance* hd_recv(int from) {
    HammingDistance* res = hd_create(0,0);
    net_recv(from, res, sizeof(HammingDistance), 0);
    return res;
}