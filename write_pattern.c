/*
 *  Copyright (c) 2014 NetApp, Inc.
 *  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#define BLOCK_SIZE 512
#define BLOCKS 512
#define BUF_SIZE (BLOCKS * BLOCK_SIZE)

int
main(int argc, char ** argv)
{
	size_t bytes;
	char * endptr;
	FILE * file;
	char * buffer;
	struct timeval tv_start;
	struct timeval tv_end;
	int block;
	size_t bytes_left;
	double elapsed;

	if (3 != argc) {
		fprintf(stderr, "Usage: %s <bytes> <filename>\n", argv[0]);
		exit(2);
	}

	errno = 0;
	bytes = strtoul(argv[1], &endptr, 10);
	if (0 != errno || '\0' != *endptr) {
		fprintf(stderr, "Invalid number of bytes: %s\n", argv[1]);
		exit(2);
	}

	buffer = (char *) malloc(BUF_SIZE);
	if (0 == buffer) {
		fprintf(stderr, "Not enough memory\n");
		exit(1);
	}

	memset(buffer, 0x55, BUF_SIZE);

	gettimeofday(&tv_start, 0);

	file = fopen(argv[2], "w");
	if (0 == file) {
		fprintf(stderr, "Failed to open file for writing: %s\n", argv[2]);
		exit(1);
	}

	bytes_left = bytes;
	block = 0;
	while (0 < bytes_left) {
		int i;
		char * ptr = buffer;
		size_t to_write = bytes_left;
		size_t nb;

		for (i = 0; i < BLOCKS; i++) {
			*((int *) ptr) = block++;
			ptr += BLOCK_SIZE;
		}

		if (to_write > BUF_SIZE) {
			to_write = BUF_SIZE;
		}

		nb = fwrite(buffer, 1, to_write, file);
		if (nb != to_write) {
			perror("Write failed");
			exit(1);
		}

		bytes_left -= nb;
	}

	fsync(fileno(file));

	fclose(file);

	gettimeofday(&tv_end, 0);

	elapsed = (double) (tv_end.tv_sec - tv_start.tv_sec);
	if (tv_start.tv_usec <= tv_end.tv_usec) {
		elapsed += 0.000001 * (tv_end.tv_usec - tv_start.tv_usec);
	} else {
		elapsed -= 0.000001 * (tv_start.tv_usec - tv_end.tv_usec);
	}

	if (0 >= elapsed) {
		fprintf(stderr, "Internal error\n");
		exit(1);
	}

	printf("%u\n", (unsigned int) (bytes / elapsed));

	return 0;
}
