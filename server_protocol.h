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
#include <dirent.h>

#define MAX_USERNAME_SIZE 25
#define MAX_PASSWORD_SIZE 25
#define MAX_CLIENTS 15
#define MAX_FILES_PER_CLIENT 15
#define MAX_FILE_SIZE 512
#define MAX_PATH_NAME 500
#define MAX_FILE_NAME 50
#define MAX_COMMAND_NAME 13


typedef struct user_t {
	const char* user_name;
	const char* password;
	const char* dir_path;
} User;



#endif /* SERVER_PROTOCOL_H_ */
