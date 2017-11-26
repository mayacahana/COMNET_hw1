/*
 * network.h
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_USERNAME_SIZE 25
#define MAX_PASSWORD_SIZE 25
#define MAX_CLIENTS 15
#define MAX_FILES_PER_CLIENT 15
#define MAX_FILE_SIZE 512
#define MAX_PATH_NAME 500
#define MAX_FILE_NAME 50
#define MAX_COMMAND_NAME 15


typedef enum messageType{
	LOGIN_DETAILS,
	LIST_OF_FILES,
	DELETE_FILE,
	ADD_FILE,
	GET_FILE,
	QUIT,
	ERROR
} MessageType;

typedef struct Header_t{
	MessageType type;
	short arg1len;
	short arg2len;
} MessageHeader;

typedef struct Message_t{
	MessageHeader header;
	char* arg1;
	char* arg2;
	int fromClient;//1 - from client, 0 - from server
} Message;



int send_command(int sckt, Message* msg_to_sent);

int receive_command(int sckt, Message* msg_recieved);

#endif
