#include "Common.h"
#include "Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "TripleArray.h"
#include "RandArray.h"
#include "OpenValArray.h"

int MACkeyShare;
TripleArray* tripleArray;
int seed;
RandArray* randArray;
OpenValArray* openValArray;

int client_desc, socket_desc, dealer_desc;

void testServerFunctionalities() {
	// Testing Iris receiving here
	Iris* iris = iris_recv(client_desc);
	if (VERBOSE==2) iris_print(iris);
		
	// Reading Iris from file; the same sent from Client at the moment
	Iris* iris2 = iris_read(IRIS_SERVER);
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
	debug_hammingDistClear(iris, iris2);

	// Arbitrary Iris modification
	for (i=0; i<iris->size; i++) iris->iriscode[i]=1;

	printf("Authentication check after arbitrarily modifying one of the two irises...\n");
	debug_hammingDistClear(iris, iris2);

	destroyShares(shares);
	iris_destroy(iris);
	iris_destroy(iris2);
	exit(0);
}

void testSPDZ() {
	int i;
	Iris* clientIrisClear = iris_read(IRIS_CLIENT); // for operation check purposes only
	Iris* serverIrisClear = iris_read(IRIS_SERVER);		
	Iris** shares = genIrisShares(serverIrisClear);

	Iris* serverIris = shares[0];
	iris_send(shares[1], client_desc);
	iris_send(shares[0], client_desc); // will send even the Server's share, for checks clientside

	Iris* clientOtherShares  = iris_recv(client_desc);
	Iris* clientSelfShares   = iris_recv(client_desc);

	// terminates if communication failure
	for (i=0; i<clientIrisClear->size; i++) {
		assert(clientOtherShares->iriscode[i]+clientSelfShares->iriscode[i]==clientIrisClear->iriscode[i]);
		assert(clientOtherShares->mask[i]+clientSelfShares->mask[i]==clientIrisClear->mask[i]);
	}

	debug_hammingDistClear(serverIrisClear, clientIrisClear);
	spdz_hammingDist(serverIris, clientOtherShares, tripleArray, SERVER, client_desc, openValArray);

	
	iris_destroy(serverIrisClear);
	iris_destroy(clientIrisClear); // TEMP
	destroyShares(shares); 	      // will also destroy serverIris!
	iris_destroy(clientSelfShares);
	iris_destroy(clientOtherShares);
}

void protocol() {
	Iris* serverIrisClear = iris_read(IRIS_SERVER);		
	Iris** shares = genIrisShares(serverIrisClear);

	Iris* serverIris = shares[0];
	iris_send(shares[1], client_desc);

	Iris* clientIris = iris_recv(client_desc);

	spdz_hammingDist(serverIris, clientIris, tripleArray, SERVER, client_desc, openValArray);

	iris_destroy(serverIrisClear);
	destroyShares(shares); 	      // will also destroy serverIris!
	iris_destroy(clientIris);
}

int main(int argc, char** argv) {
	socket_desc = bindPort(SERVER_PORT);
	dealer_desc	= connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[SERVER] Connection to Dealer was successful.\n");
	client_desc = connectionFrom(socket_desc);
	
	// TD will send both the MAC key share and triples shares
	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[SERVER] Received MACkeyShare: %d\n", MACkeyShare);

	tripleArray=tripleArray_recv(dealer_desc);
	printf("[SERVER] Received %d multiplicative triple shares, %d free space.\n", MAX_TRIPLES, tripleArray->freeSpace);

	seed=recvInt(dealer_desc);
	printf("[SERVER] Received seed %d.\n", seed);

	randArray=randArray_create(tripleArray->size);
	randArray_populate(randArray, seed);
	randArray_print(randArray);
	printf("[SERVER] Generated random values for the MAC check.\n");

	openValArray=openValArray_create(tripleArray->size);
	printf("[SERVER] Initialized the open values structure.\n");

	if (VERBOSE) tripleArray_print(tripleArray);
	
	// if (DEBUG) testServerFunctionalities();
	if (CONVERTER) shrinkIrisFile("irisServer_raw.txt", IRIS_SERVER);

	// Same as Client
	if (DEBUG) testSPDZ();
	else protocol();

	openValArray_print(openValArray);
	spdz_MACCheck(openValArray, randArray, MACkeyShare, dealer_desc);

	openValArray_destroy(openValArray);
	randArray_destroy(randArray);
	tripleArray_destroy(tripleArray);

	close(socket_desc);
	close(client_desc);
	close(dealer_desc);
	
	return 0;
}