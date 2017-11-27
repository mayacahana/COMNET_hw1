/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include "client_protocol.h"

/*
 * @param pointer to string and size_t variable n
 * removes last n bits from the string.
 */
void chopN(char* str, size_t n) {
	if (!str) {
		return;
	}
	size_t len = strlen(str);
	if (n > len)
		return;
	memmove(str, str + n, len - n + 1);
}

/*
 * sends server username and password
 * returns 0 if succeeds, otherwise returns 1
 */
int defineUser(int scket) {
	int status = 1;
	Message* user_msg = (Message*) malloc(sizeof(Message));
	if (!user_msg) {
		printf("malloc failed\n");
		fflush(NULL );
		status = 0;
	}

	//get username and password
	while (status && user_msg->header.type != QUIT) {
		int userMaxLen = MAX_USERNAME_SIZE + strlen("User: ");
		int passMaxLen = MAX_PASSWORD_SIZE + strlen("Password: ");
		char* fullUsername = (char*) malloc(sizeof(char) * (userMaxLen));
		fgets(fullUsername, userMaxLen, stdin);
		char userPrefix[6];
		strncpy(userPrefix, fullUsername, 6);
		userPrefix[6] = '\0';
		int userFlag = strcmp(userPrefix, "User: ");
		chopN(fullUsername, strlen("User: "));
		fullUsername[strlen(fullUsername)-1] = '\0';
		if (strcmp(fullUsername, "quit\n") != 0) {
				createMessageCommand(user_msg, LOGIN_DETAILS, fullUsername);
				char* fullPassword = (char*) malloc(sizeof(char) * (passMaxLen));
				fgets(fullPassword, userMaxLen, stdin);
				char passwordPrefix[10];
				strncpy(passwordPrefix, fullPassword, 10);
				passwordPrefix[10] = '\0';
				int passFlag = strcmp(passwordPrefix, "Password: ");
				printf("%s\n%s\n", userPrefix, passwordPrefix);
				if (strcmp(fullPassword, "quit\n") == 0) {
						createQuitCommand(user_msg);
				}
				else if(passFlag || userFlag){
					printf("Wrong prefix of 'User:' or 'Password:'");
					printf("u = %d, p = %d", userFlag, passFlag);
					printf("user = %s\n", fullUsername);
				}
				else{
					status = send_command(scket, user_msg);
					if (!status){
						Message* pass_msg = (Message*) malloc(sizeof(Message));
						chopN(fullPassword, strlen("Password: "));
						fullPassword[strlen(fullPassword)-1] = '\0';
						createMessageCommand(pass_msg, LOGIN_DETAILS, fullPassword);
						status = send_command(scket, pass_msg);
						receive_command(scket, pass_msg);
						if (pass_msg->header.type == INVALID_LINE){
							status = 1;
						}
						else{
							status = 0;
							printMessageArg(pass_msg);
						}
						free(pass_msg);
					}
					else{
						printf("Error in sending user name\n");
					}

				}
		}
		else{
			createQuitCommand(user_msg);
			send_command(scket, user_msg);
			status = 1;
		}
	}
	free(user_msg);
	return status;
}

void createMessageCommand(Message* m, MessageType type,
		char* prefix) {
	if (m == NULL )
		return;
	m->header.type = type;
	strcpy(m->arg1,prefix);
	m->header.arg1len = strlen(prefix)+1;
	//chopN(commandStr, strlen(prefix));
	return;
}

void createQuitCommand(Message* m){
	createMessageCommand(m, QUIT, "00");
}

int listOfFilesCommand(Message* m, char* commandStr, int mySocketfd) {
	printf("im in list_of_files\n");
	createMessageCommand(m, LIST_OF_FILES, "list");
	int status = send_command(mySocketfd, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		return 0;
	}
	status = receive_command(mySocketfd, m);
	if (status) {
		printf("error in receiving message\n");
	}
	printMessageArg(m);
	free(m);
	return status;
}

int deleteFileCommand(Message* m, char* commandStr, char* inputPrefix,
		 int mySocket) {
	printf("im in deleteFiles\n");
	createMessageCommand(m, DELETE_FILE, inputPrefix);
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(inputPrefix);
		free(m);
		return 0;
	}
	free(inputPrefix);
	status = receive_command(mySocket, m);
	if (status) {
		printf("error in receiving message\n");
	}
	printf("%s", m->arg1);
	free(inputPrefix);
	free(m);
	return 0;
}

int addFileCommand(Message* m, char* commandStr, char* inputPrefix,
		 int mySocket) {
	printf("im in addFiles\n");
	createMessageCommand(m, ADD_FILE, inputPrefix);
	char* buffer = addFileClientSide(m->arg1);
	if (buffer == NULL ) {
		free(inputPrefix);
		return 0;
	}
	//strcpy(m->arg1, m->arg2);
	//m->arg2 = buffer;
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		free(inputPrefix);
		return 0;
	}
	status = receive_command(mySocket, m);
	if (status)
		printf("error in receiving message\n");
	printf("%s", m->arg1);
	free(inputPrefix);
	free(m);
	return 0;
}

int getFileCommand(Message* m, char* commandStr, char* inputPrefix, int mySocket) {
	printf("im in get_file\n");
	createMessageCommand(m, GET_FILE, inputPrefix);
	//char* path_to_save = (char*) malloc(strlen(m->arg2));
	//strcpy(path_to_save, m->arg2);
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		free(inputPrefix);
		return 0;
	}
	status = receive_command(mySocket, m);
	if (status && (m->header.type != ERROR)){}
	//	getFileClientSide(path_to_save, m->arg1);
	else {
		printf("error in receiving message\n");
		free(inputPrefix);
		free(m);
		return 0;
	}
	return 0;
}

int sendClientCommand(char* commandStr, int mySocketfd) {
	printf("in sendClientCommand\n");
	int status = 0;
	char* inputPrefix = (char*) malloc(
			sizeof(char) * (strlen("list_of_files") + 2));
	strcpy(inputPrefix, commandStr);
	inputPrefix[strlen("list_of_files")] = '\0';
	Message* m = (Message*) malloc(sizeof(Message));

	printf("inputPrefix: %s\n", inputPrefix);//DELETE THIS

	if (strcmp(inputPrefix, "list_of_files") == 0) {
		free(inputPrefix);
		return listOfFilesCommand(m, commandStr, mySocketfd);
	}
	printf("salami deleteFile\n");
	inputPrefix[11] = '\0';
	if (strcmp(inputPrefix, "delete_file\n") == 0) {
		printf("salami2\n");
		return deleteFileCommand(m, commandStr, inputPrefix,
				mySocketfd);
	}
	printf("salami addfile\n");
	inputPrefix[8] = '\0';
	if (strcmp(inputPrefix, "add_file\n") == 0) {
		return addFileCommand(m, commandStr, inputPrefix,
				mySocketfd);
	}

	printf("salami getFile\n");
	if (strcmp(inputPrefix, "get_file\n") == 0) {
		return getFileCommand(m, commandStr, inputPrefix,
				mySocketfd);
	}
printf("salami quit\n");
	inputPrefix[4] = '\0';
	if (strcmp(inputPrefix, "quit") == 0) {
		printf("we got quit\n");
		if (close(mySocketfd) == -1) {
			printf("close() failed\n");
			free(inputPrefix);
		}

		free(m);
		free(inputPrefix);
		return 1;
	}
	printf("end\n");
	free(m);
	free(inputPrefix);
	return 0;
}

char* addFileClientSide(char* filePath) {
	FILE* fp = fopen(filePath, "r");
	char* buffer = (char*) malloc(MAX_FILE_SIZE);
	if (!buffer) {
		printf("malloc error\n");
		return NULL ;
	}
	fread(buffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	return buffer;
}

void getFileClientSide(char* filePath, char* fileBuffer) {
	FILE *file = fopen(filePath, "w");
	if (file == NULL ) {
		printf("File Not Opened\n");
		return;
	}
	fwrite(fileBuffer, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	printf("File Added Successfully");
	return;
}

int client_start(char* hostname, int port) {
	if (hostname == NULL ) {
		char* hostname = (char*) malloc(sizeof(char) * 11);
		strcpy(hostname, "localhost");
//port = 1337;
		port = 5566;
	} else if (port == 0)
//port = 1337;
		port = 5566;

	int status, serverSocket;
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd < 0) {
		printf("Could not create socket\n");
		return 1;
	}
	struct sockaddr_in server_addr;

	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(hostname, &server_addr.sin_addr);
	serverSocket = connect(socketfd, (struct sockaddr*) &server_addr,
			sizeof(server_addr));
	if (serverSocket < 0) {
		close(socketfd);
		printf("Connection failed \n");
		return 1;
	}
	fflush(NULL );
	Message* msg = (Message*) calloc(sizeof(Message),1);
	status = receive_command(socketfd, msg);
	if (status){
		printf("problem in greeting\n");
	}
	else{
		printMessageArg(msg);
		status = defineUser(socketfd);
		printf("status: %d\n", status);
		fflush(NULL );
	}
	char* inputStr;
	while (status == 0) {
		inputStr = (char*) malloc(sizeof(char) * (MAX_COMMAND_NAME + 1));
		fgets(inputStr, MAX_COMMAND_NAME + 1, stdin);
		status = sendClientCommand(inputStr, socketfd);
		free(inputStr);
	}
	if (close(socketfd) == -1) {
		printf("close() failed\n");
		free(inputStr);
		return 1;

	}

	return status;
}

