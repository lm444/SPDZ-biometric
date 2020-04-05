#include "Common.h"
#include "Iris.h"
#include "Communication.h"

int MACkeyShare;
const MultTriple* MultTriples;

int server_desc, dealer_desc;

int main(int argc, char** argv) {
	int ret;

	server_desc=connectionTo(SERVER_ADDR, SERVER_PORT);
	printf("[CLIENT] Connection to Server was successful.\n");

	dealer_desc=connectionTo(DEALER_ADDR, DEALER_PORT);
	printf("[CLIENT] Connection to Dealer was successful.\n");

	MACkeyShare=recvMACkeyShare(dealer_desc);
	printf("[CLIENT] Received MACkeyShare: %d\n", MACkeyShare);

	if (CONVERTER) shrinkIrisFile();
	if (DEBUG) {
	 	Iris* iris = readIris(IRIS_CLIENT);
		printIris(iris);
		sendIris(iris, server_desc);
		printf("Sent iris of size: %d.\n", iris->size);
		destroyIris(iris);
	}
	
	return 0;
}
