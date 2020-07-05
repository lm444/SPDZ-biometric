#include "Common.h"
#include "./structures/Iris.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "./structures/TripleArray.h"
#include "./structures/RandArray.h"
#include "./structures/OpenValArray.h"
#include "Party.h"
#include "./structures/HammingDist.h"

int MACkeyShare;
TripleArray* tripleArray;
int seed;
RandArray* randArray;
OpenValArray* openValArray;
Party* client;

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

	client = party_create(CLIENT, MACkeyShare, server_desc, tripleArray, randArray, openValArray);

	printf("[CLIENT] Printing server iris before MAC is populated...\n");
	iris_print(serverOtherShares);
	spdz_genIrisMACShares(serverOtherShares, client);
	printf("[CLIENT] Printing server iris after MAC is populated...\n");
	iris_print(serverOtherShares);
	printf("[CLIENT] Printing client iris before MAC is populated...\n");
	iris_print(clientIris);
	spdz_genIrisMACShares(clientIris, client);
	printf("[CLIENT] Printing client iris after MAC is populated...\n");
	iris_print(clientIris);

	HammingDistance* hd_clear = debug_hd(clientIrisClear, serverIrisClear);
	hd_send(hd_clear, dealer_desc);
	HammingDistance* hd_share = spdz_hd(serverOtherShares, clientIris, client);
	HammingDistance* hd_other = spdz_MACCheck(client, dealer_desc, hd_share);
	printf("[CLIENT] Successfully computed the HD: %d, %d\n", hd_share->num+hd_other->num, hd_share->den+hd_other->den);

	hd_destroy(hd_other);
	hd_destroy(hd_clear);
	hd_destroy(hd_share);
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
	client = party_create(CLIENT, MACkeyShare, server_desc, tripleArray, randArray, openValArray);
	spdz_hd(serverIris, clientIris, client);

	iris_destroy(clientIrisClear);
	destroyShares(shares); 	      // will also destroy clientIris!
	iris_destroy(serverIris); 
}

int main(int argc, char** argv) {
	server_desc=net_connect(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=net_connect(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=net_recvMACkeyShare(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	tripleArray=tripleArray_recv(dealer_desc);
	printf("[CLIENT] Received %d multiplicative triple shares, %d free space.\n", MAX_TRIPLES, tripleArray->freeSpace);
	
	seed=net_recvInt(dealer_desc);
	printf("[CLIENT] Received seed %d.\n", seed);

	randArray=randArray_create(tripleArray->size);
	randArray_populate(randArray, seed);
	if (VERBOSE) randArray_print(randArray);
	printf("[CLIENT] Generated random values for the MAC check.\n");

	openValArray=openValArray_create(tripleArray->size);
	printf("[CLIENT] Initialized the open values structure.\n");

	if (VERBOSE) tripleArray_print(tripleArray);
 
	if (CONVERTER) shrinkIrisFile("irisClient_raw.txt", IRIS_CLIENT);

	// if (DEBUG) testClientFunctionalities();

	if (DEBUG) testSPDZ();
	else protocol();

	if (VERBOSE) openValArray_print(openValArray);

	party_destroy(client);
	close(server_desc);
	close(dealer_desc);
	return 0;
}
