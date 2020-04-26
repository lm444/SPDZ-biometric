#include "Common.h"
#include "Communication.h"
#include "SPDZ.h"

/* Every share will be structured in an array of two fields
   arr[0] will be related to the Server
   arr[1] will be related to the Client */

#define DEBUG_RAND_MAX 100000

int MACkeyShares[2];
MultTriple* MultTripleShares[2];

// Idea: move the following two methods to a separate module, probably SPDZ.c

void generateTriples() {
    int i;
    int randA, randB, randC;

    MultTripleShares[SERVER] = (MultTriple*) malloc(sizeof(MultTriple)*MAX_TRIPLES);
    MultTripleShares[CLIENT] = (MultTriple*) malloc(sizeof(MultTriple)*MAX_TRIPLES);
    
    // Will compact a bit references.

    MultTriple* serverShares = MultTripleShares[SERVER];
    MultTriple* clientShares = MultTripleShares[CLIENT];

    for (i=0; i<MAX_TRIPLES; i++) {
        randA = rand()/DEBUG_RAND_MAX;
        randB = rand()/DEBUG_RAND_MAX;
        randC = randA*randB;

        serverShares[i].a = randA-rand()/DEBUG_RAND_MAX;
        serverShares[i].b = randB-rand()/DEBUG_RAND_MAX;
        serverShares[i].c = randC-rand()/DEBUG_RAND_MAX;

        clientShares[i].a = randA-serverShares[i].a;
        clientShares[i].b = randB-serverShares[i].b;
        clientShares[i].c = randC-serverShares[i].c;
    }

    // Generic checks over generation of triples or prints. 
    // The loop will run if either DEBUG or VERBOSE is not zero.
    // Will not check by default since it would weigh down computation.

    if (DEBUG|VERBOSE) {
        for (i=0; i<MAX_TRIPLES; i++) {
            if (VERBOSE==2) {
                printf("MultTripleShares[SERVER][%d] = %d, %d, %d", i, serverShares[i].a, serverShares[i].b, serverShares[i].c);
                printf("\n");
                printf("MultTripleShares[CLIENT][%d] = %d, %d, %d", i, clientShares[i].a, clientShares[i].b, clientShares[i].c);
            }
            if (DEBUG) assert((serverShares[i].a+clientShares[i].a)*(serverShares[i].b+clientShares[i].b)==serverShares[i].c+clientShares[i].c);
            if (VERBOSE==2) printf("\n");
        }
    }
}

void destroyTriples(MultTriple** triples) {
    free(triples[0]);
    free(triples[1]);
}

int main() {
    // TD will have a connection with both the Server and the Client.
    // IMPORTANT: it is mandatory to run Dealer -> Server -> Client

    int socket_desc = bindPort(DEALER_PORT);
    int server_desc = connectionFrom(socket_desc);
    printf("[TRUSTED DEALER] Inbound connection from Server\n");
    int client_desc = connectionFrom(socket_desc);
    printf("[TRUSTED DEALER] Inbound connection from Client\n");

    int MAC = rand();
    MACkeyShares[SERVER] = MAC-rand();
    MACkeyShares[CLIENT] = MAC-MACkeyShares[SERVER];
    assert(MACkeyShares[SERVER]+MACkeyShares[CLIENT]==MAC);
    printf("[TRUSTED DEALER] MAC shares generated.\n");

    printf("[TRUSTED DEALER] Sending now MAC key shares...\n");
    sendMACkeyShare(MACkeyShares[SERVER], server_desc);
    sendMACkeyShare(MACkeyShares[CLIENT], client_desc);
    printf("[TRUSTED DEALER] Sent MAC key shares: Server -> %d, Client -> %d.\n", MACkeyShares[SERVER], MACkeyShares[CLIENT]);
 
    printf("[TRUSTED DEALER] Will now generate %d multiplicative triples...\n", MAX_TRIPLES);
    generateTriples();
    printf("[TRUSTED DEALER] Multiplicative triples generated.\n");

    sendTripleShares(MultTripleShares[SERVER], MAX_TRIPLES, server_desc);
    sendTripleShares(MultTripleShares[CLIENT], MAX_TRIPLES, client_desc);

    destroyTriples(MultTripleShares);
    close(socket_desc);
    close(server_desc);
    close(client_desc);
}