/*
 * network.h
 *
 *  Created on: Nov 14, 2017
 *      Author: mayacahana
 */

#ifndef NETWORK_H_
#define NETWORK_H_
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
};
#endif /* NETWORK_H_ */

