#ifndef DEFINES_H_
#define DEFINES_H_

// network-related
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define DEALER_ADDR "127.0.0.1"
#define DEALER_PORT 5556

#define BUFFER_SIZE 4096 // buffer size for send/recv

// defines of options for spdz_mult
#define MULT_SAVEOPEN 1

// maximum value allowed for different data
#define MAXVAL_TRIPLE   10
#define MAXVAL_RANDARR  10
#define MAXVAL_MACKEY   10
#define MAXVAL_IRISCODE 10

/* different levels of VERBOSE:
   1 - Just some minor additional print
   2 - Includes lenghty prints (care!)
*/
#define VERBOSE 	1
#define DEBUG 		1

// usually used in debug prints for VERBOSE=1
// prints the first DEBUG_PRINTELEMS 
// and last DEBUG_PRINTELEMS elements of an array
#define DEBUG_PRINTELEMS 4

#define DEBUG_IRIS_CLIENT "irisClient.txt"
#define DEBUG_IRIS_SERVER "irisServer.txt"

// triples that will be generated and sent
#define MAX_TRIPLES 5000000

// buffer for conversion
#define IO_BUFFER_SIZE 65536

#define FILENAME_MAXCONVLEN 256
#define FILENAME_MAXLEN     128
#define FILENAME_CONVSTR    "conv_"

// IDs for parties in SPDZ
#define ID_SERVER 0
#define ID_CLIENT 1

#endif