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
	return 0;
}
