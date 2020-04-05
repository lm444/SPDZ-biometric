#ifndef COMMUNICATIONS_H_
#define COMMUNICATIONS_H_

int connectionTo(const char* IPaddr, int port);
int bindPort(int port);
int connectionFrom(int port);
void sendMACkeyShare(int MACkeyShare, int to);
int recvMACkeyShare(int from);

#endif