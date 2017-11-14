/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include "client_protocol.h"



char* getUserDetails(){
	char* userDetails[40];
	int read = getline(&userDetails, 40, stdin);
	if (read != -1){
		puts(userDetails);
		return userDetails;
	}
	printf("No Line Read");
	return NULL;
}


void chopN(char *str, size_t n)
	{
	    assert(n != 0 && str != 0);
	    size_t len = strlen(str);
	    if (n > len)
	        return;  // Or: n = len;
	    memmove(str, str+n, len - n + 1);
	}

/*
 * sends server username and password
 * returns 0 if succeeds, otherwise returns 1
 */
int defineUser(){
	Message* m = (Message*)malloc(sizeof(Message));
	int status = 0;
	//get username and password
	char* fullUsername = getUserDetails();
	char* fullPassword = getUserPassword();
	char* userPrefix[7];
	char* passwordPrefix[11];
	strncpy(userPrefix, fullUsername);
	strncpy(passwordPrefix, fullPassword);
	userPrefix[7] = "/0"; // "User: " if in correct format
	passwordPrefix[11] = "/0";// "Password: " if in correct format
	//check if input is in correct format and create message
	if ((strcmp(userPrefix, "User: ") == 0) && (strcmp(passwordPrefix, "Password: ") == 0)){
		char* username[26], password[26];
		username = chopN(fullUsername, 6);
		password = chopN(fullPassword, 10);
		m->MessageType = LOGIN_DETAILS;
		m->arg1 = username;
		m->arg2 = password;
		m->fromClient = 1;
		status = send_message(m);
	}else{
		printf("wrong format");// fix this
		status = 1;
	}
	free(fullUsername);
	free(fullPassword);
	free(userPrefix);
	free(passwordPrefix);
	return status;
}


Message* createMessage(char* commandStr, MessageType type, char* prefix){
	Message* m = (Message*)malloc(sizeof(Message));
	if (m == NULL)
		return NULL;
	m->MessageType = type;
	m->fromClient = 1;
	m->arg1 = prefix;
	m->arg2 = chopN(commandStr, strlen(prefix));
	return m*;
}


int sendCommand(char* commandStr){
	int status = 0;
	char* inputPrefix[14];
	strncpy(inputPrefix, commandStr);
	inputPrefix[14] = "/0";
	if (strcmp(inputPrefix, "list_of_files") == 0){
		Message* m = createMessage(commandStr, LIST_OF_FILES, inputPrefix);
		if (m != NULL){
			status = send_message(m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[11] = "/0";
	if (strcmp(inputPrefix, "delete_file") == 0){
		Message* m = createMessage(commandStr, DELETE_FILE, inputPrefix);
		if (m != NULL){
			status = send_message(m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[8] = "/0";
	if (strcmp(inputPrefix, "add_file") == 0){
		Message* m = createMessage(commandStr, ADD_FILE, inputPrefix);
		if (m != NULL){
			status = send_message(m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	if (strcmp(inputPrefix, "get_file") == 0){
		Message* m = createMessage(commandStr, GET_FILE, inputPrefix);
		if (m != NULL){
			status = send_message(m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[4] = "/0";
	if (strcmp(inputPrefix, "quit") == 0){
		Message* m = createMessage(commandStr, QUIT, inputPrefix);
		if (m != NULL){
			status = send_message(m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	else printf("wrong command format");
	return 1;
}


int startClient(char* hostname, int port){
	if (hostname == NULL){
		hostname = malloc(sizeof(byte)*15);
		hostname = "localhost";
		port = 2235;
	}
	else if (port == 0)
		port = 2235;

	int status, newsocketfd;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in my_addr, client_addr;
	socklen_t client_size = sizeof(client_addr);
	if (socketfd < 0) {
		printf("Could not create socket\n");
		return 1;
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons( port );
	dest_addr.sin_addr = htonl(0x8443FC64);

	connect(socketfd, (struct sockaddr*) &dest_addr, sizeof(struct sockaddr));


	int status = 0;
	status = defineUser();
	while (status == 0){



	}


	return status;
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







}

