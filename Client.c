#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"

int MACkeyShare;
MultTriple* MultTripleShares;

int server_desc, dealer_desc;

int main(int argc, char** argv) {
	int ret;

	server_desc=connectionTo(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	MultTripleShares=recvTripleShares(dealer_desc);
	printf("[CLIENT] Received %d multiplicative triple shares.\n", MAX_TRIPLES);
	
	if (VERBOSE==2) {
		int i;
		for (i=0; i<MAX_TRIPLES; i++) {
			printf("MultTripleShares[%d] = %d %d %d\n", i, MultTripleShares[i].a, MultTripleShares[i].b, MultTripleShares[i].c);
		}
	}
	
	if (CONVERTER) shrinkIrisFile();
	if (DEBUG) {
	 	Iris* iris = readIris(IRIS_CLIENT);
		printIris(iris);
		sendIris(iris, server_desc);
		printf("Sent iris of size: %d.\n", iris->size);
		destroyIris(iris);
	}
	
	close(server_desc);
	close(dealer_desc);
	return 0;
}
