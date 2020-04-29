#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "MultTriple.h"

int MACkeyShare;
MultTriple* MultTripleShares;

int client_desc, socket_desc, dealer_desc;

void testServerFunctionalities() {
	// Testing Iris receiving here
	Iris* iris = recvIris(client_desc);
	if (VERBOSE==2) printIris(iris);
		
	// Reading Iris from file; the same sent from Client at the moment
	Iris* iris2 = readIris(IRIS_SERVER);
	if (iris->size!=iris2->size) {
		printf("Mismatching sizes. Terminating... "); exit(1);
	}
		
	// Checking the two Irises are indeed the same and well represented
	int i;
	for (i=0; i<iris->size; i++) {
		if (iris->mask[i]!=iris2->mask[i]) {
			printf("Mismatching mask. Terminating... "); exit(1);
		}
		if (iris->iriscode[i]!=iris2->iriscode[i]) {
			printf("Mismatching iriscode. Terminating... "); exit(1);
		}
	}
	printf("Communication was successful.\n");

	printf("Generating shares of iris...\n");
	Iris** shares = genIrisShares(iris);
	if (VERBOSE==2) printShares(shares);

	// Checking Hamming Distance between Irises (here: same iris)
	printf("Now will try authentication check between two same irises...\n");
	AuthCheckClear(iris, iris2);

	// Arbitrary Iris modification
	for (i=0; i<iris->size; i++) iris->iriscode[i]=1;

	printf("Authentication check after arbitrarily modifying one of the two irises...\n");
	AuthCheckClear(iris, iris2);

	destroyShares(shares);
	destroyIris(iris);
	destroyIris(iris2);
	exit(0);
}

int main(int argc, char** argv) {
	socket_desc = bindPort(SERVER_PORT);
	dealer_desc	= connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[SERVER] Connection to Dealer was successful.\n");
	client_desc = connectionFrom(socket_desc);
	
	// TD will send both the MAC key share and triples shares
	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[SERVER] Received MACkeyShare: %d\n", MACkeyShare);

	MultTripleShares=recvTripleShares(dealer_desc);
	printf("[SERVER] Received %d multiplicative triple shares.\n", MAX_TRIPLES);

	// Printing all the shares of triples received (VERBOSE==2)
	// or up to PRINT_ELEMS triples (VERBOSE==1) 
	if (VERBOSE) {
		int i, max=PRINT_ELEMS;
		int reverse=MAX_TRIPLES-1;
		if (VERBOSE==2) max = MAX_TRIPLES;
		for (i=0; i<max; i++, reverse--) {
			printf("MultTripleShares[%d] = %d %d %d\n", i, MultTripleShares[i].a, MultTripleShares[i].b, MultTripleShares[i].c);
			printf("MultTripleShares[%d] = %d %d %d\n", reverse, MultTripleShares[reverse].a, MultTripleShares[reverse].b, MultTripleShares[reverse].c);
		}
	}
	
	// if (DEBUG) testServerFunctionalities();
	if (CONVERTER) shrinkIrisFile("irisServer_raw.txt", IRIS_SERVER);

	// Same as Client

	Iris* clientIrisClear = readIris(IRIS_CLIENT); // for operation check purposes only

	Iris* serverIrisClear = readIris(IRIS_CLIENT);		
	Iris** shares = genIrisShares(serverIrisClear);

	Iris* serverIris = shares[0];
	sendIris(shares[1], client_desc);

	Iris* clientIris = recvIris(client_desc);

	AuthCheckClear(serverIrisClear, clientIrisClear);
	spdz_hamming_dist(serverIris, clientIris, MultTripleShares, SERVER, client_desc);

	destroyIris(serverIrisClear);
	destroyIris(clientIrisClear);
	destroyShares(shares); 		// will also destroy serverIris!
	destroyIris(clientIris);
	free(MultTripleShares);

	close(socket_desc);
	close(client_desc);
	close(dealer_desc);
	
	return 0;
}