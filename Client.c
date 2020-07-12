#include "Common.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "Converter.h"
#include "./structures/TripleArray.h"
#include "./structures/RandArray.h"
#include "./structures/OpenValArray.h"
#include "./structures/Iris.h"
#include "./structures/HammingDist.h"

char* iris_client;
int MACkeyShare;
TripleArray* tripleArray;
int seed;
RandArray* randArray;
OpenValArray* openValArray;
Party* client;

int server_desc, dealer_desc;

void testClientFunctionalities() {
	Iris* iris = iris_read(DEBUG_IRIS_CLIENT);
	if (VERBOSE==2) iris_print(iris);

	iris_send(iris, server_desc);
	iris_destroy(iris);
	exit(0);
}

// debug ONLY with DEBUG_IRIS_SERVER and DEBUG_IRIS_CLIENT.
void testSPDZ() {
	int i;
	Iris* serverIrisClear = iris_read(DEBUG_IRIS_SERVER);		// for operation check purposes only

	Iris* clientIrisClear = iris_read(DEBUG_IRIS_CLIENT);
	Iris** shares = iris_genShares(clientIrisClear);

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

	client = party_create(ID_CLIENT, MACkeyShare, server_desc, tripleArray, randArray, openValArray);

	printf("[CLIENT] Printing server iris before MAC is populated...\n");
	iris_print(serverOtherShares);
	spdz_genIrisMACShares(client, serverOtherShares);
	printf("[CLIENT] Printing server iris after MAC is populated...\n");
	iris_print(serverOtherShares);
	printf("[CLIENT] Printing client iris before MAC is populated...\n");
	iris_print(clientIris);
	spdz_genIrisMACShares(client, clientIris);
	printf("[CLIENT] Printing client iris after MAC is populated...\n");
	iris_print(clientIris);

	HammingDistance* hd_clear = debug_hd(clientIrisClear, serverIrisClear);
	hd_send(hd_clear, dealer_desc);
	HammingDistance* hd_share = spdz_hd(client, serverOtherShares, clientIris);
	HammingDistance* hd_other = spdz_MACCheck(client, dealer_desc, hd_share);
	printf("[CLIENT] Successfully computed the HD: %d, %d\n", hd_share->num+hd_other->num, hd_share->den+hd_other->den);

	hd_destroy(hd_other);
	hd_destroy(hd_clear);
	hd_destroy(hd_share);
	iris_destroy(clientIrisClear);
	iris_destroy(serverIrisClear); 
	iris_destroy(shares[0]); 
	iris_destroy(shares[1]); 
	free(shares);
	iris_destroy(serverOtherShares); 
	iris_destroy(serverSelfShares);
}

void protocol() {
	/* clientIris will be the shares of the Client's iris 
	   serverIris will be the shares of the Server's iris 
	   we will operate on those two for SPDZ; clientIrisClear won't be used */
	   
	Iris* clientIrisClear = iris_read(iris_client);
	Iris** shares = iris_genShares(clientIrisClear);

	Iris* clientIris = shares[0];
	iris_send(shares[1], server_desc);

	Iris* serverIris = iris_recv(server_desc);
	client = party_create(ID_CLIENT, MACkeyShare, server_desc, tripleArray, randArray, openValArray);

	printf("[CLIENT] Populating server iris' MAC...\n");
	spdz_genIrisMACShares(client, serverIris);
	printf("[CLIENT] Populating client iris' MAC...\n");
	spdz_genIrisMACShares(client, clientIris);

	HammingDistance* hd_share = spdz_hd(client, serverIris, clientIris);
	HammingDistance* hd_other = spdz_MACCheck(client, dealer_desc, hd_share);

	printf("[CLIENT] Successfully computed the HD: %d, %d\n", hd_share->num+hd_other->num, hd_share->den+hd_other->den);

	hd_destroy(hd_other);
	hd_destroy(hd_share);
	iris_destroy(clientIrisClear);
	iris_destroy(shares[0]); 
	iris_destroy(shares[1]); 
	free(shares);
	iris_destroy(serverIris); 
}

int main(int argc, char** argv) {
	if (argc<2) {
		printf("usage: ./client <irisfile>\n");
		exit(1);
	}

	iris_client = conv_if(argv[1]);

	server_desc=net_connect(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=net_connect(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=net_recvInt(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	tripleArray=tripleArray_recv(dealer_desc);
	printf("[CLIENT] Received %d multiplicative triple shares.\n", MAX_TRIPLES);
	
	seed=net_recvInt(dealer_desc);
	printf("[CLIENT] Received seed %d.\n", seed);

	randArray=randArray_create(tripleArray->size);
	randArray_populate(randArray, seed);
	if (VERBOSE) randArray_print(randArray);
	printf("[CLIENT] Generated random values for the MAC check.\n");

	openValArray=openValArray_create(tripleArray->size);
	printf("[CLIENT] Initialized the open values structure.\n");

	if (VERBOSE) tripleArray_print(tripleArray);
 
	// if (DEBUG) testClientFunctionalities();

	if (DEBUG) testSPDZ();
	else protocol();

	if (VERBOSE) openValArray_print(openValArray);

	party_destroy(client);
	close(server_desc);
	close(dealer_desc);
	return 0;
}
