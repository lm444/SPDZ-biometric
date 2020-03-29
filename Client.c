#include "Common.h"
#include "Iris.h"

int main(int argc, char** argv) {
	int ret, server_desc;
	struct sockaddr_in server_addr = {0};

	server_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (server_desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	ret = connect(server_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Connect");

	printf("Connection successful.\n");

	if (CONVERTER) shrinkIrisFile();
	if (DEBUG) {
	 	Iris* iris = readIris(IRIS_CLIENT);
		printIris(iris);
		sendIris(iris, server_desc);
		printf("Sent iris of size: %d.\n", iris->size);
	}
	
	return 0;
}
