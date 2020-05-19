#ifndef COMMUNICATIONS_H_
#define COMMUNICATIONS_H_

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define DEALER_ADDR "127.0.0.1"
#define DEALER_PORT 5556

int net_connect(const char* IPaddr, int port);
int net_bind(int port);
int net_accept(int port);

int net_send(int to, void* data, int size, int flags);
int net_recv(int from, void* buf, int size, int flags);

void net_sendMACkeyShare(int MACkeyShare, int to);
int net_recvMACkeyShare(int from);
void net_sendInt(int share, int to);
int net_recvInt(int from);

#endif