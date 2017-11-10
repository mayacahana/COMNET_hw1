/*
 * server_protocol.h
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#ifndef SERVER_PROTOCOL_H_
#define SERVER_PROTOCOL_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef struct user_t {
	const char* user_name;
	const char* password;
	const char* dir_path;
} User;

#endif /* SERVER_PROTOCOL_H_ */
