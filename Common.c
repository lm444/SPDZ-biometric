#include "Common.h"

int valid_features[] = {
    1600, 1920, 2560, 3360, 5600, 5760, 6400
};

void handle_error(const char* msg) {
	perror(msg);
	exit(1);
}

int check_features(int n) {
    int i;
    if (VERBOSE) printf("Checking %d against the accepted number of features.\n", n);
    for (i=0; i<sizeof(valid_features)/sizeof(int); i++) {
        if (n == 2*valid_features[i]) return 0;
    }
    return -1;
}