#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"

int MACkeyShare;
MultTriple* MultTripleShares;

int server_desc, dealer_desc;

void testClientFunctionalities() {
	if (CONVERTER) shrinkIrisFile();
	Iris* iris = readIris(IRIS_CLIENT);
	if (VERBOSE) printIris(iris);

	sendIris(iris, server_desc);
	destroyIris(iris);
	exit(0);
}

int main(int argc, char** argv) {
	server_desc=connectionTo(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	MultTripleShares=recvTripleShares(dealer_desc);
	printf("[CLIENT] Received %d multiplicative triple shares.\n", MAX_TRIPLES);
	
	if (VERBOSE) {
		int i, max=PRINT_ELEMS;
		if (VERBOSE==2) max = MAX_TRIPLES;
		for (i=0; i<max; i++) {
			printf("MultTripleShares[%d] = %d %d %d\n", i, MultTripleShares[i].a, MultTripleShares[i].b, MultTripleShares[i].c);
		}
	}

	if (DEBUG) testClientFunctionalities();
	
	/* clientIris will be the shares of the Client's iris 
	   serverIris will be the shares of the Server's iris 
	   We will operate on those two for SPDZ; originalIris won't be used */

	Iris* originalIris = readIris(IRIS_CLIENT);
	Iris** shares = genIrisShares(originalIris);

	Iris* clientIris = shares[0];
	sendIris(shares[1], server_desc);

	Iris* serverIris = recvIris(server_desc);

	AuthCheckClear(originalIris, originalIris);
	spdz_hamming_dist(serverIris, clientIris, MultTripleShares, CLIENT, server_desc);

	destroyIris(originalIris);
	destroyShares(shares); 		// will also destroy clientIris!
	destroyIris(serverIris);
	free(MultTripleShares);

	close(server_desc);
	close(dealer_desc);
	return 0;
}
