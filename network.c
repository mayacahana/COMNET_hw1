/*
 * network.c
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */

int send_command(int sckt, Message msg_to_sent) {
	int bytesLeft = len(msg_to_sent.arg1) + len(msg_to_sent.arg2) + 1; /* how many we have left to send */
	int total = 0; /* how many bytes we've sent */
	int n, len = bytesLeft;
	Message msg;
	while (total < bytesLeft) {
		n = send(sckt, &msg_to_sent + total, bytesLeft, 0);
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

int recieve_command(int sckt, Message msg_) {

}
