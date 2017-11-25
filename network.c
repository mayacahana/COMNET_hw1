/*
 * network.c
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */
#include "network.h"

int send_command(int sckt, Message* msg_to_sent) {
	int bytesLeft =  sizeof(msg_to_sent);
	int total = 0; /* how many bytes we've sent */
	int n, len = bytesLeft;
	while (total < bytesLeft) {
		n = send(sckt, msg_to_sent + total, bytesLeft, 0);
		printf("sent %d bytes\n, socket = %d\n", n, sckt);
		if (n < 0) {
			printf("Send message with send() failed. %s\n", strerror(errno));
			return 1;
		}
		total += n;
		bytesLeft -= n;
	}
	if (total != len) {
		printf("Sent wrong amount of bytes in send().");
		return 1;
	}
	return 0;
}

int receive_command(int sckt, Message* msg_recieved) {
	int total = 0;
	int bytesLeft = sizeof(msg_recieved);
	int n, len = bytesLeft;
	char* buffer;
	buffer = (char *)malloc(sizeof(char)*MAX_FILE_SIZE+sizeof(Message));
	while (total < bytesLeft) {
		printf("Im before recv \n");
		n = recv(sckt, (void*) buffer, bytesLeft, 0);
		print("bytes sent = %d\n", n);
		if (n < 0) {
			printf("Receive message with recv() failed.\n %s", strerror(errno));
			return 1;
		}
		total += n;
		bytesLeft -= n;
	}
	printf("noe total=bytes left\n");
	if (total != len) {
		printf("Receive wrong amount of bytes in recv().");
		return 1;
	}
	strcpy(msg_recieved->arg1, buffer);
	free(buffer);
	if (total > MAX_FILE_SIZE) {
		printf("The message is too big. ");
		return 1;
	}
	return 0;
}

