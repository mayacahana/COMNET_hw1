/*
 * network.c
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */
#include "network.h"

int sendAll(int socket, void*buffer, int* len) {
	printf("Im in send all\n");
	int total = 0;
	int bytesLeft = *len;
	int n;
	while (total < *len) {
		n = (int) send(socket, (void*)((long)buffer + total), (size_t) bytesLeft, 0);
		if (n < 0) {
			break;
		}
		total += n;
		bytesLeft -= n;
	}
	*len = total;
	return n == -1 ? -1 : 0;
}
int send_command(int sckt, Message* msg_to_send) {
//	int len = HEADER_SIZE + msg_to_send->header.arg1len+ msg_to_send->header.arg2len;
	printf("I'm in send command\n");
	Message network_msg;
	network_msg.header.arg1len = htons(msg_to_send->header.arg1len);
	network_msg.header.arg2len = htons(msg_to_send->header.arg2len);
	network_msg.header.type = htons(msg_to_send->header.type);
	printf("before memcpy\n");
	network_msg.arg1 = msg_to_send->arg1;
	network_msg.arg2 = msg_to_send->arg2;
	//memcpy(network_msg.arg1, msg_to_send->arg1, msg_to_send->header.arg1len+1);
	//memcpy(network_msg.arg2, msg_to_send->arg2, msg_to_send->header.arg2len+1);
	printf("after mem\n");
	int arg1_len = (int) network_msg.header.arg1len;
	int arg2_len = (int) network_msg.header.arg2len;
	int header_len = sizeof(MessageHeader);
	printf("before sent\n");
	if (sendAll(sckt, &network_msg.header, &header_len)) {
		printf("%s\n", strerror(errno));
		printf("Bytes sent: %d \n", arg1_len);
		return 1;
	}
	if (sendAll(sckt, network_msg.arg1, &arg1_len)) {
		printf("%s\n", strerror(errno));
		printf("Bytes sent: %d \n", arg1_len);
		return 1;
	}
	if (arg2_len > 0) {
		if (sendAll(sckt, network_msg.arg2, &arg2_len)) {
			printf("%s\n", strerror(errno));
			printf("Bytes sent: %d \n", arg2_len);
			return 1;
		}
	}

	return 0;
}

int receiveAll(int socket, void* buffer, int* len) {
	int total = 0;
	int bytesLeft = *len;
	int n;
	while (total < *len) {
		n = (int) recv(socket, (void*) ((long) buffer + total),
				(size_t) bytesLeft, 0);
		if (n < 0) {
			break;
		}
		total += n;
		bytesLeft -= n;
	}
	*len = total;
	return n == -1 ? -1 : 0;
}

int receive_command(int sckt, Message* msg_received) {
	//recieve the Messageeader first
	int len_header = sizeof(MessageHeader);
	if (receiveAll(sckt, &msg_received->header, &len_header)) {
		printf("%s\n", strerror(errno));
		printf("Bytes recieved: %d \n", len_header);
		return 1;
	}
	msg_received->header.arg1len = ntohs(msg_received->header.arg1len);
	msg_received->header.arg2len = ntohs(msg_received->header.arg2len);
	msg_received->header.type = ntohs(msg_received->header.type);
	if ((msg_received->header.arg1len + msg_received->header.arg2len)
			> MAX_DATA_SIZE) {
		printf("Recieve command failed- msg too big");
		return 1;
	}
	//receive arg1
	int len_arg1 = msg_received->header.arg1len;
	msg_received->arg1 = (char*) malloc(sizeof(char*)*len_arg1);
	if (receiveAll(sckt, &msg_received->arg1, &len_arg1)) {
		printf("%s\n", strerror(errno));
		printf("Bytes recieved: %d \n", len_header);
		return 1;
	}
	//receive arg2
	int len_arg2 = msg_received->header.arg2len;
	if (len_arg2 > 0) {
		msg_received->arg1 = (char*) malloc(sizeof(char*)*len_arg2);
		if (receiveAll(sckt, &msg_received->arg2, &len_arg2)) {
			printf("%s\n", strerror(errno));
			printf("Bytes recieved: %d \n", len_header);
			return 1;
		}
	}

	return 0;

}
