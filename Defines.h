// network-related
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5555

#define DEALER_ADDR "127.0.0.1"
#define DEALER_PORT 5556

#define BUFFER_SIZE 4096 // buffer size for send/recv

// settings for spdz_mult
#define MULT_SAVEOPEN 1

// maximum value allowed for different data
#define MAXVAL_TRIPLE   RAND_MAX
#define MAXVAL_RANDARR  RAND_MAX
#define MAXVAL_MACKEY   RAND_MAX
#define MAXVAL_IRISCODE RAND_MAX

// usually used in debug prints for VERBOSE=1
// prints the first DEBUG_PRINTELEMS 
// and last DEBUG_PRINTELEMS elements of an array
#define DEBUG_PRINTELEMS 10

/* Different levels of VERBOSE:
   1 - Just some minor additional print
   2 - Includes lenghty prints (care!)
*/
#define VERBOSE 	0
#define DEBUG 		0

// triples that will be generated and sent
#define MAX_TRIPLES 5000000

// threshold of comparison
// still unused atm
#define THRESHOLD 0.25

// conversion
#define IO_BUFFER_SIZE 65536

#define FILENAME_MAXCONVLEN 256
#define FILENAME_MAXLEN     128
#define FILENAME_CONVSTR    "conv_"