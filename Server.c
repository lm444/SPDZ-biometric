#include "Common.h"
#include "Iris.h"
#include "Communication.h"

int MACkeyShare;

int main(int argc, char** argv) {
	int ret, client_desc, socket_desc, dealer_desc;

	socket_desc = bindPort(SERVER_PORT);
	dealer_desc	= connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[SERVER] Connection to Dealer was successful.\n");
	client_desc = connectionFrom(socket_desc);
	

	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[SERVER] Received MACkeyShare: %d\n", MACkeyShare);

	// 30.03.2020: just a generic test for the communication primitives.
	if (DEBUG) {
		Iris* iris = recvIris(client_desc);
		printIris(iris);
		printf("Received iris of size: %d.\n", iris->size);
		Iris* iris2 = readIris(IRIS_SERVER);
		if (iris->size!=iris2->size) {
			printf("Mismatching sizes. Terminating... "); exit(1);
		}
		int i;
		for (i=0; i<iris->size/2; i++) {
			if (iris->mask[i]!=iris2->mask[i]) {
				printf("Mismatching mask. Terminating... "); exit(1);
			}
			if (iris->iriscode[i]!=iris2->iriscode[i]) {
				printf("Mismatching iriscode. Terminating... "); exit(1);
			}
		}
		printf("Communication was successful.\n");
		destroyIris(iris);
		destroyIris(iris2);
		
	}

	return 0;
}