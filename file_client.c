/*
 * file_client.c
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "client_protocol.h"

#define DEFAULT_HOSTNAME "localhost"
#define DEFAULT_PORT 1337

int main(int argc, char* argv[]) {
	const char* hostname = DEFAULT_HOSTNAME;
	int port = DEFAULT_PORT;
	struct stat directory;
	if (argc > 3) {
		printf("Invalid arguments. usage: [[hostname][port]]");
		return 1;
	}
	if (argc > 1) { // including hostname
		hostname = argv[1];
		if (argc == 3) {
			port = atoi(argv[2]);
		}
	}

	client_start(hostname, port);
	return 0;
}
