#ifndef COMMUNICATIONS_H_
#define COMMUNICATIONS_H_

#include "SPDZ.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define DEALER_ADDR "127.0.0.1"
#define DEALER_PORT 5556

int connectionTo(const char* IPaddr, int port);
int bindPort(int port);
int connectionFrom(int port);
void sendMACkeyShare(int MACkeyShare, int to);
int recvMACkeyShare(int from);
void sendTripleShares(MultTriple* triples, int numTriples, int to);
MultTriple* recvTripleShares(int from);

#endif