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

// returns 0 if hd1 and hd2 have the same num and den
// -1 otherwise
int hd_cmp(HammingDistance* hd1, HammingDistance* hd2) {
    if (hd1->num==hd2->num && hd1->den==hd2->num) return 0;
    return -1;
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