#include "Common.h"
#include "Communication.h"
#include "./structures/TripleArray.h"
#include "HammingDist.h"

#define MAXVAL_MAC 10

/* Every share will be structured in an array of two fields
   arr[0] will be related to the Server
   arr[1] will be related to the Client */

int MACkeyShares[2];
TripleArray** TripleShares;

int main() {
    // Offline phase: will generate triples shares and MAC key shares before connections
    int ret;

    int MACKey = rand()%MAXVAL_MAC;
    printf("[TRUSTED DEALER] Generated MAC key: %d.\n", MACKey);
    MACkeyShares[SERVER] = (MACKey-(rand()-RAND_MAX/2))%MAXVAL_MAC;
    MACkeyShares[CLIENT] = MACKey-MACkeyShares[SERVER];
    assert(MACkeyShares[SERVER]+MACkeyShares[CLIENT]==MACKey);
    printf("[TRUSTED DEALER] Generated MAC shares: %d, %d.\n", MACkeyShares[SERVER], MACkeyShares[CLIENT]);

    printf("[TRUSTED DEALER] Will now generate %d multiplicative triples...\n", MAX_TRIPLES);
    TripleShares = generateTriples(MAX_TRIPLES, MACKey);
    printf("[TRUSTED DEALER] Multiplicative triples generated.\n");


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
    printf("[TRUSTED DEALER] Sent MAC key shares.\n");

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

    if (DEBUG) {
        HammingDistance* hd_client_clear = hd_recv(client_desc);
        HammingDistance* hd_server_clear = hd_recv(server_desc);
        assert(hd_client_clear->num == hd_server_clear->num);
        assert(hd_client_clear->den == hd_server_clear->den);
        printf("[TRUSTED DEALER] Calculated HD in SPDZ: NUM %d | DEN %d\n", 
                hd_client_clear->num, hd_client_clear->den);
        hd_destroy(hd_client_clear);
        hd_destroy(hd_server_clear);
    }

    HammingDistance* hd_client = hd_recv(client_desc);
    HammingDistance* hd_server = hd_recv(server_desc);

    printf("[TRUSTED DEALER] Calculated HD in SPDZ: NUM %d | DEN %d\n", 
            hd_client->num+hd_server->num, hd_client->den+hd_server->den);

    int sigma_client = recvInt(client_desc);
    int sigma_server = recvInt(server_desc);

    printf("[TRUSTED DEALER] Detected sigma sum: %d (0 = correct).\n", sigma_client+sigma_server);

    if (sigma_client+sigma_server != 0) {
        printf("[TRUSTED DEALER] ABORTING COMPUTATION: SOMEBODY CHEATED.\n");
        exit(1);
    }

    hd_destroy(hd_client);
    hd_destroy(hd_server);
 
    close(socket_desc);
    close(server_desc);
    close(client_desc);
}