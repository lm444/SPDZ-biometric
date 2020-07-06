#include "Common.h"

char* conv_ofname(const char* input_file) {   
    char* ret = (char*) malloc(FILENAME_MAXCONVLEN*sizeof(char));
    strcpy(ret, FILENAME_CONVSTR);
    strcpy(ret+strlen(FILENAME_CONVSTR), input_file);
    return ret;
}

// checks whether the input iris file is valid.
// if it isn't, attempts to convert to a valid format.
// on success, returns a file descriptor of the file
// else terminates the program.
char* conv_if(const char* input_file) {
	int fd, ret;
	fd = open(input_file, O_RDONLY);
	if (fd<0) handle_error("[CONV] Error while opening input file");

	char buf[IO_BUFFER_SIZE];
	char res[IO_BUFFER_SIZE];

	int readBytes=0, validBytes=0;

	while (1) {
		ret = read(fd, buf+readBytes, 1);
        if (ret<0 && errno==EINTR) continue;
		if (ret<0) handle_error("[CONV] Error while reading from input file");
		if (ret==0) break;
		if (buf[readBytes]=='0' || buf[readBytes]=='1') {
			res[validBytes]=buf[readBytes];
			validBytes++;
		}
		readBytes++;
	}

	buf[readBytes]='\0';
	res[validBytes]='\0';

	ret = check_features(validBytes);
	if (ret==0) {
		printf("[CONV] Detected a valid iris file.\n");
		close(fd);
		return (char*) input_file;
	}

	int writtenBytes=0;

	char* output_file = conv_ofname(input_file);
	int outfd;
	outfd = open(output_file, O_WRONLY|O_CREAT, 0640);
	if (outfd<0) handle_error("[CONV] Error while opening output file");

	while (writtenBytes<validBytes) {
		ret = write(outfd, res+writtenBytes, validBytes-writtenBytes);
		if (ret<0 && errno==EINTR) continue;
		if (ret<0) handle_error("[CONV] Error while writing on output file");
		writtenBytes+=ret;
	}

    ret = check_features(validBytes);
    if (ret<0) {
        printf("[CONV] The input file isn't valid. Please double-check it.\n");
        exit(1);
    }
	
	printf("[CONV] Successfully converted the iris file.\n");

	printf("[CONV] Input  | strlen: %ld; read: %d\n", strlen(buf), readBytes);
	printf("[CONV] Output | strlen: %ld; read: %d\n", strlen(res), validBytes);

	printf("[CONV] Converted %s to %s successfully.\n", input_file, output_file);

    ret = close(fd); 
    if (ret<0) handle_error("[CONV] Error while closing input file");
    ret = close(outfd);
    if (ret<0) handle_error("[CONV] Error while closing output file");

	return output_file;
}