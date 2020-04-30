#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "MultTriple.h"

int MACkeyShare;
MultTriple* MultTripleShares;

int server_desc, dealer_desc;

void testClientFunctionalities() {
	Iris* iris = readIris(IRIS_CLIENT);
	if (VERBOSE==2) printIris(iris);

	sendIris(iris, server_desc);
	destroyIris(iris);
	exit(0);
}

void testSPDZ() {
	int i;
	Iris* serverIrisClear = readIris(IRIS_SERVER);		// for operation check purposes only

	Iris* clientIrisClear = readIris(IRIS_CLIENT);
	Iris** shares = genIrisShares(clientIrisClear);

	Iris* clientIris = shares[0];
	sendIris(shares[1], server_desc);
	sendIris(shares[0], server_desc);

	Iris* serverOtherShares = recvIris(server_desc);
	Iris* serverSelfShares  = recvIris(server_desc);

	// terminates if communication failure
	for (i=0; i<clientIrisClear->size; i++) {
		assert(serverOtherShares->iriscode[i]+serverSelfShares->iriscode[i]==serverIrisClear->iriscode[i]);
		assert(serverOtherShares->mask[i]+serverSelfShares->mask[i]==serverIrisClear->mask[i]);
	}

	AuthCheckClear(clientIrisClear, serverIrisClear);
	spdz_hamming_dist(serverOtherShares, clientIris, MultTripleShares, CLIENT, server_desc);

	destroyIris(clientIrisClear);
	destroyIris(serverIrisClear); // TEMP
	destroyShares(shares); 	      // will also destroy clientIris!
	destroyIris(serverOtherShares); 
	destroyIris(serverSelfShares);
}

void protocol() {
	/* clientIris will be the shares of the Client's iris 
	   serverIris will be the shares of the Server's iris 
	   We will operate on those two for SPDZ; clientIrisClear won't be used */
	   
	Iris* clientIrisClear = readIris(IRIS_CLIENT);
	Iris** shares = genIrisShares(clientIrisClear);

	Iris* clientIris = shares[0];
	sendIris(shares[1], server_desc);

	Iris* serverIris = recvIris(server_desc);

	spdz_hamming_dist(serverIris, clientIris, MultTripleShares, CLIENT, server_desc);

	destroyIris(clientIrisClear);
	destroyShares(shares); 	      // will also destroy clientIris!
	destroyIris(serverIris); 
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
		int reverse = MAX_TRIPLES-1;
		if (VERBOSE==2) max = MAX_TRIPLES;
		for (i=0; i<max; i++, reverse--) {
			printf("MultTripleShares[%d] = %d %d %d\n", i, MultTripleShares[i].a, MultTripleShares[i].b, MultTripleShares[i].c);
			printf("MultTripleShares[%d] = %d %d %d\n", reverse, MultTripleShares[reverse].a, MultTripleShares[reverse].b, MultTripleShares[reverse].c);
		}
	}

	if (CONVERTER) shrinkIrisFile("irisClient_raw.txt", IRIS_CLIENT);

	// if (DEBUG) testClientFunctionalities();

	if (DEBUG) testSPDZ();
	else protocol();

	free(MultTripleShares);
	close(server_desc);
	close(dealer_desc);
	return 0;
}
