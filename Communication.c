#include "Common.h"
#include "MultTriple.h"

// Connection primitives; wrappers

#define BUFFER_SIZE 4096

int bindPort(int port) {
    int ret, socket_desc;

    struct sockaddr_in server_addr = {0};

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0) handle_error("Socket create");

    int so_reuseaddropt = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddropt, sizeof(int)) < 0) handle_error("SO_REUSEADDR setup failed");

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

int sendTo(int to, void* data, int size, int flags) {                   // if size > BUFFER_SIZE, will split into multiple sends
    int ret, sentBytes=0;
    int currWindow = BUFFER_SIZE;

    while (sentBytes<size) {
        if (size-sentBytes < currWindow) currWindow = size-sentBytes;
        ret = send(to, data+sentBytes, currWindow, flags);
        if (ret<0 && errno==EINTR) continue;
        else if (ret<0) handle_error("sendTo failed");
        sentBytes += ret;
    }

    if (VERBOSE && sentBytes>4) printf("Sent %d bytes.\n", sentBytes);
    else if (VERBOSE==2) printf("Sent %d bytes.\n", sentBytes);         // long prints - even 4bytes interactions
    return sentBytes;
}

int recvFrom(int from, void* buf, int size, int flags) {                // if size > BUFFER_SIZE, will split into multiple recvs
    int ret, recvBytes=0;
    int currWindow = BUFFER_SIZE;

    while (recvBytes<size) {
        if (size-recvBytes < currWindow) currWindow = size-recvBytes;
        ret = recv(from, buf+recvBytes, currWindow, flags);
        if (ret<0 && errno==EINTR) continue;
        else if (ret<0) handle_error("recvFrom failed");
        recvBytes += ret;
    }
    
    if (VERBOSE && recvBytes>4) printf("Received %d bytes.\n", recvBytes); 
    else if (VERBOSE==2) printf("Received %d bytes.\n", recvBytes);     // long prints - even 4bytes interactions
    return recvBytes; 
}

// Application-specific methods

void sendIntShare(int share, int to) {
    sendTo(to, &share, sizeof(int), 0);
}

int recvIntShare(int from) {
    int res;
    recvFrom(from, &res, sizeof(int), 0);
    return res;
}

// Detached from the generic sendIntShare to simplify modification
// for a different representation of the MAC key (e.g. uint64)
void sendMACkeyShare(int MACkeyShare, int to) {
    sendIntShare(MACkeyShare, to);
}

int recvMACkeyShare(int from) {
    return recvIntShare(from);
}
