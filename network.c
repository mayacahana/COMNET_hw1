/*
 * network.c
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */
#include "network.h"

int send_command(int sckt, Message* msg_to_sent) {
	int bytesLeft = strlen(msg_to_sent->arg1) + strlen(msg_to_sent->arg2) + 1; /* how many we have left to send */
	int total = 0; /* how many bytes we've sent */
	int n, len = bytesLeft;
	while (total < bytesLeft) {
		n = send(sckt, msg_to_sent + total, bytesLeft, 0);
		if (n < 0) {
			printf("Send message with send() failed.");
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
	int bytesLeft = sizeof(msg_recieved->type) + strlen(msg_recieved->arg1)
			+ strlen(msg_recieved->arg2);
	int n, len = bytesLeft;
	while (total < bytesLeft) {
		n = recv(sckt, msg_recieved, bytesLeft, 0);
		if (n < 0) {
			printf("Receive message with recv() failed.");
			return 1;
		}
		total += n;
		bytesLeft -= n;
	}
	if (total != len){
		printf("Receive wrong amount of bytes in recv().");
		return 1;
	}
	if (total > MAX_FILE_SIZE) {
		printf("The message is too big. ");
		return 1;
	}
	return 0;
}
