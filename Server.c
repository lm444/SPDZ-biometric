#include "Common.h"
#include "Communication.h"
#include "SPDZ.h"
#include "Debug.h"
#include "Converter.h"
#include "./structures/Iris.h"
#include "./structures/TripleArray.h"
#include "./structures/RandArray.h"
#include "./structures/OpenValArray.h"
#include "./structures/HammingDist.h"

char* iris_server;
int MACkeyShare;
TripleArray* tripleArray;
int seed;
RandArray* randArray;
OpenValArray* openValArray;
Party* server;

int client_desc, socket_desc, dealer_desc;

void testServerFunctionalities() {
	// testing Iris recv here
	Iris* iris = iris_recv(client_desc);
	if (VERBOSE==2) iris_print(iris);
		
	// reading Iris from file; the same sent from Client at the moment
	Iris* iris2 = iris_read(DEBUG_IRIS_SERVER);
	if (iris->size!=iris2->size) {
		printf("Mismatching sizes. Terminating... "); exit(1);
	}
		
	// checking the two Irises are indeed the same and well represented
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
	Iris** shares = iris_genShares(iris);
	if (VERBOSE==2) {
		iris_print(shares[0]);
		iris_print(shares[1]);
	}

	// checking Hamming Distance between Irises (here: same iris)
	printf("Now will try authentication check between two same irises...\n");
	debug_hd(iris, iris2);

	// arbitrary Iris modification
	for (i=0; i<iris->size; i++) iris->iriscode[i]=1;

	printf("Authentication check after arbitrarily modifying one of the two irises...\n");
	debug_hd(iris, iris2);

	iris_destroy(shares[0]); 
	iris_destroy(shares[1]); 
	free(shares);
	iris_destroy(iris);
	iris_destroy(iris2);
	exit(0);
}

// debug ONLY with DEBUG_IRIS_SERVER and DEBUG_IRIS_CLIENT.
void testSPDZ() {
	int i;
	Iris* clientIrisClear = iris_read(DEBUG_IRIS_CLIENT); // for operation check purposes only
	Iris* serverIrisClear = iris_read(DEBUG_IRIS_SERVER);		
	Iris** shares = iris_genShares(serverIrisClear);

	Iris* serverIris = shares[0];
	iris_send(shares[1], client_desc);
	iris_send(shares[0], client_desc); // will send even the Server's shares, for checks clientside

	Iris* clientOtherShares  = iris_recv(client_desc);
	Iris* clientSelfShares   = iris_recv(client_desc);


	// terminates if communication failure
	for (i=0; i<clientIrisClear->size; i++) {
		assert(clientOtherShares->iriscode[i]+clientSelfShares->iriscode[i]==clientIrisClear->iriscode[i]);
		assert(clientOtherShares->mask[i]+clientSelfShares->mask[i]==clientIrisClear->mask[i]);
	}

	server = party_create(ID_SERVER, MACkeyShare, client_desc, tripleArray, randArray, openValArray);


	if (VERBOSE) {
		printf("[SERVER] Printing client iris before MAC is populated...\n");
		iris_print(clientOtherShares);
	}

	spdz_genIrisMACShares(server, clientOtherShares);

	if (VERBOSE) {
		printf("[SERVER] Printing client iris after MAC is populated...\n");
		iris_print(clientOtherShares);

		printf("[SERVER] Printing server iris before MAC is populated...\n");
		iris_print(serverIris);
	}

	spdz_genIrisMACShares(server, serverIris);

	if (VERBOSE) {
		printf("[SERVER] Printing server iris after MAC is populated...\n");
		iris_print(serverIris);
	}

	HammingDistance* hd_clear = debug_hd(serverIrisClear, clientIrisClear);
	hd_send(hd_clear, dealer_desc);
	HammingDistance* hd_share = spdz_hd(server, serverIris, clientOtherShares);
	HammingDistance* hd_other = spdz_MACCheck(server, dealer_desc, hd_share);
	printf("[SERVER] Successfully computed the HD: %d, %d\n", hd_share->num+hd_other->num, hd_share->den+hd_other->den);

	hd_destroy(hd_other);
	hd_destroy(hd_clear);
	hd_destroy(hd_share);
	iris_destroy(serverIrisClear);
	iris_destroy(clientIrisClear);
	iris_destroy(shares[0]); 
	iris_destroy(shares[1]); 
	free(shares);
	iris_destroy(clientSelfShares);
	iris_destroy(clientOtherShares);
}

void protocol() {
	Iris* serverIrisClear = iris_read(iris_server);		
	Iris** shares = iris_genShares(serverIrisClear);

	Iris* serverIris = shares[0];
	iris_send(shares[1], client_desc);

	Iris* clientIris = iris_recv(client_desc);

	server = party_create(ID_SERVER, MACkeyShare, client_desc, tripleArray, randArray, openValArray);

	printf("[SERVER] Populating client iris' MAC...\n");
	spdz_genIrisMACShares(server, clientIris);
	printf("[SERVER] Populating server iris' MAC ...\n");
	spdz_genIrisMACShares(server, serverIris);

	HammingDistance* hd_share = spdz_hd(server, serverIris, clientIris);
	HammingDistance* hd_other = spdz_MACCheck(server, dealer_desc, hd_share);
	printf("[SERVER] Successfully computed the HD: %d, %d\n", hd_share->num+hd_other->num, hd_share->den+hd_other->den);

	hd_destroy(hd_other);
	hd_destroy(hd_share);
	iris_destroy(serverIrisClear);
	iris_destroy(shares[0]); 
	iris_destroy(shares[1]); 
	free(shares);
	iris_destroy(clientIris);
}

int main(int argc, char** argv) {
	if (!DEBUG) {
		if (argc<2) {
			printf("usage: ./server <irisfile>\n");
			exit(1);
		}
		iris_server = conv_if(argv[1]);
	}

	socket_desc = net_bind(SERVER_PORT);
	dealer_desc	= net_connect(DEALER_ADDR, DEALER_PORT);
	printf("[SERVER] Connection to Dealer was successful.\n");
	client_desc = net_accept(socket_desc);
	
	// TD will send both the MAC key share and triples shares
	MACkeyShare=net_recvInt(dealer_desc);
	printf("[SERVER] Received MACkeyShare: %d\n", MACkeyShare);

	tripleArray=tripleArray_recv(dealer_desc);
	printf("[SERVER] Received %d multiplicative triple shares.\n", MAX_TRIPLES);

	seed=net_recvInt(dealer_desc);
	printf("[SERVER] Received seed %d.\n", seed);

	randArray=randArray_create(tripleArray->size);
	randArray_populate(randArray, seed);
	if (VERBOSE) randArray_print(randArray);
	printf("[SERVER] Generated random values for the MAC check.\n");

	openValArray=openValArray_create(tripleArray->size);
	printf("[SERVER] Initialized the open values structure.\n");

	if (VERBOSE) tripleArray_print(tripleArray);
	
	// if (DEBUG) testServerFunctionalities();
	
	// same as Client
	if (DEBUG) testSPDZ();
	else protocol();

	if (VERBOSE) openValArray_print(openValArray);

	party_destroy(server);
	close(socket_desc);
	close(client_desc);
	close(dealer_desc);
	
	return 0;
}