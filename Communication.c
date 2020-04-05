#include "Common.h"

int connectionTo(const char* IPaddr, int port) {
    int ret, desc;
    struct sockaddr_in server_addr = {0};
    desc = socket(AF_INET, SOCK_STREAM, 0);
	if (desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = inet_addr(IPaddr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	ret = connect(desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Connect");
    
    return desc;
}

int bindPort(int port) {
    int ret, socket_desc;

    struct sockaddr_in server_addr = {0};

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	ret = bind(socket_desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Bind");

	ret = listen(socket_desc, 5);
	if (ret<0) handle_error("Listen");

    if (VERBOSE) printf("Bound port: %d", port);
    return socket_desc;
}

int connectionFrom(int socket_desc) {
    int ret, client_desc;

    int sockaddr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr = {0};

	client_desc = accept(socket_desc, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
	if (client_desc<0) handle_error("Accept");

    if (VERBOSE) printf("Inbound connection from %s, port %d", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
    return client_desc;
}

void sendMACkeyShare(int MACkeyShare, int to) {
    send(to, &MACkeyShare, sizeof(int), 0);
}

int recvMACkeyShare(int from) {
    int res;
    recv(from, &res, sizeof(int), 0);
    return res;
}