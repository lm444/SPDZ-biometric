#include "Common.h"
#include "Communication.h"
#include "MultTriple.h"

/* Every share will be structured in an array of two fields
   arr[0] will be related to the Server
   arr[1] will be related to the Client */

int MACkeyShares[2];
MultTripleArray** MultTripleShares;

int main() {
    // Offline phase: will generate triples shares and MAC key shares before connections
    int ret;

    printf("[TRUSTED DEALER] Will now generate %d multiplicative triples...\n", MAX_TRIPLES);
    MultTripleShares = generateTriples(MAX_TRIPLES);
    printf("[TRUSTED DEALER] Multiplicative triples generated.\n");

    int MAC = rand();
    MACkeyShares[SERVER] = MAC-rand();
    MACkeyShares[CLIENT] = MAC-MACkeyShares[SERVER];
    assert(MACkeyShares[SERVER]+MACkeyShares[CLIENT]==MAC);
    printf("[TRUSTED DEALER] MAC shares generated.\n");

    // TD will have a connection with both the Server and the Client.
    // IMPORTANT: it is mandatory to run Dealer -> Server -> Client

    int socket_desc = bindPort(DEALER_PORT);
    int server_desc = connectionFrom(socket_desc);
    printf("[TRUSTED DEALER] Inbound connection from Server\n");
    int client_desc = connectionFrom(socket_desc);
    printf("[TRUSTED DEALER] Inbound connection from Client\n");

    printf("[TRUSTED DEALER] Sending now MAC key shares...\n");
    sendMACkeyShare(MACkeyShares[SERVER], server_desc);
    sendMACkeyShare(MACkeyShares[CLIENT], client_desc);
    printf("[TRUSTED DEALER] Sent MAC key shares: Server -> %d, Client -> %d.\n", MACkeyShares[SERVER], MACkeyShares[CLIENT]);

    printf("[TRUSTED DEALER] Sending now multiplicative triples key shares...\n");
    ret=sendTripleShares(MultTripleShares[SERVER]->circularArray, MAX_TRIPLES, server_desc);
    printf("[TRUSTED DEALER] Sent %d multiplicative triples to Server.\n", ret);
    ret=sendTripleShares(MultTripleShares[CLIENT]->circularArray, MAX_TRIPLES, client_desc);
    printf("[TRUSTED DEALER] Sent %d multiplicative triples to Client.\n", ret);
    
    destroyMultTripleArray(MultTripleShares[SERVER]);
    destroyMultTripleArray(MultTripleShares[CLIENT]);
    free(MultTripleShares);

    close(socket_desc);
    close(server_desc);
    close(client_desc);
}