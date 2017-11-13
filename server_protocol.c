/*
 * server_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#include "server_protocol.h"

int client_serving(int clientsocket){


}


void start_listen(User *usersArray, int numOfUsers, int port) {
	int status, newsocketfd;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in my_addr, client_addr;
	socklen_t client_size = sizeof(client_addr);
	if (socketfd < 0) {
		printf("Could not create socket\n");
		return;
	}


	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	//my_addr.sin_addr =
	status = bind(socketfd, &my_addr, sizeof(my_addr));

	if (status < 0) {
		printf("Could not bind ip to socket");
		return;
	}

	if(listen(socketfd, 1) < 0 ){
		printf("Could not listen...");
		return;
	}
	while (1) {
		newsocketfd = accept(socketfd, &client_addr, &client_size);
		if (newsocketfd < 0){
			printf("accept() not successful...");
			return;
		}
		client_serving(newsocketfd);
		close(newsocketfd);
	}
}
void start_server(char* users_file, const char* dir_path, int port) {
	//creating the main folder
	//reading input file
	FILE* usersFile = fopen(users_file, "r");

	User* usersArray = (User*) malloc(15 * sizeof(User));
	int numOfUsers = 0;
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
				User* newUser = (User*) malloc(sizeof(User));
				strcpy(newUser->user_name, user_buffer);
				sscanf(usersFile, "%s", pass_buffer);
				strcpy(newUser->password, pass_buffer);
				strcpy(newUser->dir_path, fileDirPath);
				usersArray[numOfUsers] = newUser;
				numOfUsers++;
			} else {
				printf("cannot create user directory for :%s", user_buffer);
			}
			free(user_buffer);
			free(pass_buffer);
			free(fileDirPath);

			user_buffer = (char*) malloc(sizeof(char*) * 26);
			pass_buffer = (char*) malloc(sizeof(char*) * 26);
		}
		strat_listen(usersArray);
	}
}
