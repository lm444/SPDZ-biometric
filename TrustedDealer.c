#include "Common.h"
#include "Communication.h"
#include "TripleArray.h"

#define MAXVAL_MAC 10

/* Every share will be structured in an array of two fields
   arr[0] will be related to the Server
   arr[1] will be related to the Client */

int MACkeyShares[2];
TripleArray** TripleShares;

int main() {
    // Offline phase: will generate triples shares and MAC key shares before connections
    int ret;

    printf("[TRUSTED DEALER] Will now generate %d multiplicative triples...\n", MAX_TRIPLES);
    TripleShares = generateTriples(MAX_TRIPLES);
    printf("[TRUSTED DEALER] Multiplicative triples generated.\n");

    int MAC = rand()%MAXVAL_MAC;
    MACkeyShares[SERVER] = (MAC-(rand()-RAND_MAX/2))%MAXVAL_MAC;
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
    ret=tripleArray_send(TripleShares[SERVER], MAX_TRIPLES, server_desc);
    printf("[TRUSTED DEALER] Sent %d multiplicative triples to Server.\n", ret);
    ret=tripleArray_send(TripleShares[CLIENT], MAX_TRIPLES, client_desc);
    printf("[TRUSTED DEALER] Sent %d multiplicative triples to Client.\n", ret);

    // Client and Server has to generate the same random vector.
    int seed = rand();
    printf("[TRUSTED DEALER] Sending seed %d for the random value generation (for MAC check).\n", seed);
    sendInt(seed, server_desc);
    sendInt(seed, client_desc);
    
    tripleArray_destroy(TripleShares[SERVER]);
    tripleArray_destroy(TripleShares[CLIENT]);
    free(TripleShares);

    close(socket_desc);
    close(server_desc);
    close(client_desc);
}