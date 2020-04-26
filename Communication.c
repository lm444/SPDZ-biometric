#include "Common.h"
#include "SPDZ.h"

// Connection primitives; wrappers

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

    if (VERBOSE) printf("Bound port: %d\n", port);
    return socket_desc;
}

int connectionFrom(int socket_desc) {
    int client_desc;

    int sockaddr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr = {0};

	client_desc = accept(socket_desc, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
	if (client_desc<0) handle_error("Accept");

    if (VERBOSE) printf("Inbound connection from %s, port %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
    return client_desc;
}

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

// Socket operations

int sendTo(int to, void* data, int size, int flags) {
    int ret, sentBytes=0;
    while (sentBytes<size) {
        ret = send(to, data, size, flags);
        if (ret<0 && errno==EINTR) continue;
        else if (ret<0) handle_error("sendTo failed");
        sentBytes += ret;
    }
    if (VERBOSE) printf("Sent %d bytes.\n", sentBytes);
    return sentBytes;
}

int recvFrom(int from, void* buf, int size, int flags) {
    int ret, recvBytes=0;
    while (recvBytes<size) {
        ret = recv(from, buf, size, flags);
        if (ret<0 && errno==EINTR) continue;
        else if (ret<0) handle_error("recvFrom failed");
        recvBytes += ret;
    }
    if (VERBOSE) printf("Received %d bytes.\n", recvBytes);
    return recvBytes; 
}

// Application-specific methods

// Detached from the generic sendIntShare to simplify modification
// for a different representation of the MAC key (e.g. uint64)

void sendMACkeyShare(int MACkeyShare, int to) {
    sendTo(to, &MACkeyShare, sizeof(int), 0);
}

int recvMACkeyShare(int from) {
    int res;
    recvFrom(from, &res, sizeof(int), 0);
    return res;
}

void sendTripleShares(MultTriple* triples, int numTriples, int to) {
    sendTo(to, &numTriples, sizeof(int), 0);
    sendTo(to, triples, numTriples*sizeof(MultTriple), 0);
}

MultTriple* recvTripleShares(int from) {
    int numTriples; 
    recvFrom(from, &numTriples, sizeof(int), 0);

    MultTriple* res = malloc(numTriples*sizeof(MultTriple));
    recvFrom(from, res, numTriples*sizeof(MultTriple), 0);
    return res;
}

void sendIntShare(int share, int to) {
    sendTo(to, &share, sizeof(int), 0);
}

int recvIntShare(int from) {
    int res;
    recvFrom(from, &res, sizeof(int), 0);
    return res;
}