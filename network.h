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
#include <dirent.h>

#define MAX_USERNAME_SIZE 25
#define MAX_PASSWORD_SIZE 25
#define MAX_CLIENTS 15
#define MAX_FILES_PER_CLIENT 15
#define MAX_FILE_SIZE 512
#define MAX_PATH_NAME 500
#define MAX_FILE_NAME 50
#define MAX_COMMAND_NAME 13


/*
#include "network.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "server_protocol.h"
//
//#pragma pack(push, 1)
//typedef struct
//{
//	short opcode;
//	short length;
//}message_header;
//
//typedef struct
//{
//	message_header header;
//	char data[MAX_DATA_SIZE];
//}message;
//#pragma pack(pop)

typedef struct {
	message_command;
	char command_data[MAX_DATA_SIZE]
};*/
#endif  NETWORK_H_

