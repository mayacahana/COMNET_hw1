/*
 * server_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

void start_server(char* users_file, const char* dir_path, int port) {
	//creating the main folder
	//reading input file
	FILE* usersFile = fopen(users_file, "r");

	/*struct stat dirctry;

	stat(dir_path, &dirctry);*/

	if (usersFile != NULL) {
		char* user_buffer = (char*) malloc(sizeof(char*) * 26);
		char* pass_buffer = (char*) malloc(sizeof(char*) * 26);

	//	const char* fileDirPath = (char*) malloc(sizeof(dir_path) + 27);

		while (sscanf(usersFile, "%s", user_buffer) > 0) {
			const char* fileDirPath = (char*) malloc(sizeof(dir_path) + 27);

			strcpy(fileDirPath, dir_path);
			strcat(fileDirPath, "/");
			strcat(fileDirPath, user_buffer);

			if (!mkdir(fileDirPath)) {

			}
			else {
				printf("cannot create user directory for :%s",user_buffer);
			}
			free(user_buffer);
			free(pass_buffer);
			free(fileDirPath);

			user_buffer = (char*) malloc(sizeof(char*) * 26);
			pass_buffer = (char*) malloc(sizeof(char*) * 26);

		}

	}
}
