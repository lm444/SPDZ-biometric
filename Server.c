#include "Common.h"
#include "Iris.h"

int main(int argc, char** argv) {
	int ret, socket_desc, client_desc;
	struct sockaddr_in server_addr = {0}, client_addr = {0};
	int sockaddr_len = sizeof(struct sockaddr_in);

	Iris* test= (Iris*) malloc(sizeof(Iris));

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	ret = bind(socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Bind");

	ret = listen(socket_desc, 1);
	if (ret<0) handle_error("Listen");

	client_desc = accept(socket_desc, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
	if (client_desc<0) handle_error("Accept");
	printf("Connection successful.\n");

	// 30.03.2020: just a generic test for the communication primitives.
	if (DEBUG) {
		Iris* iris = recvIris(client_desc);
		printIris(iris);
		printf("Received iris of size: %d.\n", iris->size);
		Iris* iris2 = readIris(IRIS_SERVER);
		if (iris->size!=iris2->size) {
			printf("Mismatching sizes. Terminating... "); exit(1);
		}
		int i;
		for (i=0; i<iris->size/2; i++) {
			if (iris->mask[i]!=iris2->mask[i]) {
				printf("Mismatching mask. Terminating... "); exit(1);
			}
			if (iris->iriscode[i]!=iris2->iriscode[i]) {
				printf("Mismatching mask. Terminating... "); exit(1);
			}
		}
		printf("Communication was successful.\n");
	}

	return 0;
}
