#include "Common.h"

// Communication wrappers
int net_bind(int port) {
    int ret, socket_desc;

    struct sockaddr_in server_addr = {0};

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc<0) handle_error("Socket create");

    int so_reuseaddr_opt = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr_opt, sizeof(int)) < 0) handle_error("SO_REUSEADDR setup failed");

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

int net_accept(int socket_desc) {
    int client_desc;

    int sockaddr_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_addr = {0};

	client_desc = accept(socket_desc, (struct sockaddr*) &client_addr, (socklen_t*) &sockaddr_len);
	if (client_desc<0) handle_error("Accept");

    if (VERBOSE) printf("Inbound connection from %s, port %d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
    return client_desc;
}

int net_connect(const char* IP, int port) {
    int ret, desc;
    struct sockaddr_in server_addr = {0};
    desc = socket(AF_INET, SOCK_STREAM, 0);
	if (desc<0) handle_error("Socket create");

	server_addr.sin_addr.s_addr = inet_addr(IP);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	ret = connect(desc, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));
	if (ret<0) handle_error("Connect");
    
    return desc;
}

// if size > BUFFER_SIZE, will split into multiple sends
int net_send(int to, void* data, int size, int flags) {                   
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

// if size > BUFFER_SIZE, will split into multiple recvs
int net_recv(int from, void* buf, int size, int flags) {                
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

void net_sendInt(int share, int to) {
    net_send(to, &share, sizeof(int), 0);
}

int net_recvInt(int from) {
    int res;
    net_recv(from, &res, sizeof(int), 0);
    return res;
}

// Detached from the generic sendIntShare to simplify modification
// for a different representation of the MAC key (e.g. uint64)
void net_sendMACkeyShare(int MACkeyShare, int to) {
    net_sendInt(MACkeyShare, to);
}

int net_recvMACkeyShare(int from) {
    return net_recvInt(from);
}
