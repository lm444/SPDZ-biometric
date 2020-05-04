#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "MultTriple.h"

int MACkeyShare;
MultTripleArray* MultTripleShares;

int server_desc, dealer_desc;

void testClientFunctionalities() {
	Iris* iris = iris_read(IRIS_CLIENT);
	if (VERBOSE==2) iris_print(iris);

	iris_send(iris, server_desc);
	iris_destroy(iris);
	exit(0);
}

void testSPDZ() {
	int i;
	Iris* serverIrisClear = iris_read(IRIS_SERVER);		// for operation check purposes only

	Iris* clientIrisClear = iris_read(IRIS_CLIENT);
	Iris** shares = genIrisShares(clientIrisClear);

	Iris* clientIris = shares[0];
	iris_send(shares[1], server_desc);
	iris_send(shares[0], server_desc);

	Iris* serverOtherShares = iris_recv(server_desc);
	Iris* serverSelfShares  = iris_recv(server_desc);

	// terminates if communication failure
	for (i=0; i<clientIrisClear->size; i++) {
		assert(serverOtherShares->iriscode[i]+serverSelfShares->iriscode[i]==serverIrisClear->iriscode[i]);
		assert(serverOtherShares->mask[i]+serverSelfShares->mask[i]==serverIrisClear->mask[i]);
	}

	debug_hammingDistClear(clientIrisClear, serverIrisClear);
	spdz_hammingDist(serverOtherShares, clientIris, MultTripleShares, CLIENT, server_desc);

	iris_destroy(clientIrisClear);
	iris_destroy(serverIrisClear); 
	destroyShares(shares); 	       // will also destroy clientIris!
	iris_destroy(serverOtherShares); 
	iris_destroy(serverSelfShares);
}

void protocol() {
	/* clientIris will be the shares of the Client's iris 
	   serverIris will be the shares of the Server's iris 
	   We will operate on those two for SPDZ; clientIrisClear won't be used */
	   
	Iris* clientIrisClear = iris_read(IRIS_CLIENT);
	Iris** shares = genIrisShares(clientIrisClear);

	Iris* clientIris = shares[0];
	iris_send(shares[1], server_desc);

	Iris* serverIris = iris_recv(server_desc);

	spdz_hammingDist(serverIris, clientIris, MultTripleShares, CLIENT, server_desc);

	iris_destroy(clientIrisClear);
	destroyShares(shares); 	      // will also destroy clientIris!
	iris_destroy(serverIris); 
}

int main(int argc, char** argv) {
	server_desc=connectionTo(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	MultTripleShares=tripleArray_recv(dealer_desc);
	printf("[CLIENT] Received %d multiplicative triple shares, %d free space.\n", MAX_TRIPLES, MultTripleShares->freeSpace);
	
	if (VERBOSE) tripleArray_print(MultTripleShares);

	if (CONVERTER) shrinkIrisFile("irisClient_raw.txt", IRIS_CLIENT);

	// if (DEBUG) testClientFunctionalities();

	if (DEBUG) testSPDZ();
	else protocol();

	tripleArray_destroy(MultTripleShares);

	close(server_desc);
	close(dealer_desc);
	return 0;
}
