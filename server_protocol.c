/*
 * server_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#include "server_protocol.h"

Message* createServerMessage(MessageType type, char* arg1, char* arg2) {
	Message* msg = (Message*) malloc(sizeof(Message));
	msg->fromClient = 0;
	msg->arg1 = arg1;
	msg->arg2 = arg2;
	msg->type = type;
	return msg;
}

void addFile(int clientSocket, Message* msg, User* user) {
	if (!user || !msg) {
		printf("Error in Message or User");
		return;
	}
	char pathToFile[sizeof(user->dir_path) + sizeof(msg->arg1) + 5];
	strcpy(pathToFile, user->dir_path);
	strcat(pathToFile, "/");
	strcat(pathToFile, msg->arg1);
	FILE *file = fopen(pathToFile, "w");
	Message* msgToSend;
	if (file == NULL) {
		printf("File Not Added\n");
		msgToSend = createServerMessage(msg->type, "ERROR", NULL);
		send_command(clientSocket, msgToSend);
		free(msgToSend);
		return;
	}
	fwrite(msg->arg2, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	msg = createServerMessage(msg->type, "File added", NULL);
	send_command(clientSocket, msg);
	free(msg);

}

void deleteFile(int clientSocket, Message* msg, User* user) {
	if (!user || !msg) {
		printf("Error in Message or User");
		return;
	}
	char pathToFile[sizeof(user->dir_path) + sizeof(msg->arg1) + 5];
	strcpy(pathToFile, user->dir_path);
	strcat(pathToFile, "/");
	strcat(pathToFile, msg->arg1);
	int status = remove(pathToFile);
	char* arg = (char*) malloc(sizeof(char) * 20);
	if (status == 0) {
		strcpy(arg, "File removed!");
	} else {
		strcpy(arg, "No such file exists!");
	}
	Message* msgToSend = createServerMessage(DELETE_FILE, arg, NULL);
	send_command(clientSocket, msgToSend);
	free(arg);
	free(msgToSend);
}

void sendListOfFiles(int clientSocket, User* user) {
	if (!user) {
		return;
	}
	char files_names[MAX_FILES_PER_CLIENT * MAX_FILE_NAME];
	char file_name[MAX_FILE_NAME];
	DIR *d;
	struct dirent *dir;
	d = opendir(user->dir_path);
	if (d != NULL) {
		while ((dir = readdir(d)) != NULL) {
			sprintf(file_name,"%s\n",dir->d_name);
			sprintf(files_names , "%s\n",file_name);
		}
		closedir(d);
	}
	Message* msg = createServerMessage(LIST_OF_FILES, files_names, NULL);
	send_command(clientSocket, msg);
	free(msg);
	return;
}

void sendFileToClient(int clientSocket, Message* msg, User* user) {
	// get the file name
//	char file_name[MAX_FILE_NAME];
//	int file_size;
//	char * username = user->user_name;
	FILE* fp;
	char pathToFile[strlen(user->dir_path) + strlen(msg->arg1) + 1];
	sprintf(pathToFile, "%s/%s/%s", user->dir_path,user->user_name, msg->arg1);
	puts(pathToFile);
	fp = fopen(pathToFile, "rb");
	if (fp == NULL) {
		printf("Can't open the file to send");
		free(pathToFile);
		return;
	}
	//read the file into a buffer
	char * fileBuffer = malloc(MAX_FILE_SIZE + 1);
	fread(fileBuffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	//fileBuffer[fileBuffer] = '\0';
	msg->arg1 = fileBuffer;
	msg->fromClient = 0;
	send_command(clientSocket, msg);
	free(fileBuffer);
	free(pathToFile);

}
void handleMessage(int clientSocket, Message *msg, User* user) {
	if (!msg) {
		return;
	}
	switch(msg->type) {
	case LIST_OF_FILES:
		sendListOfFiles(clientSocket, user);
		return;
	case DELETE_FILE:
		deleteFile(clientSocket, msg, user);
		return;
	case ADD_FILE:
		addFile(clientSocket, msg, user);
		return;
	case GET_FILE:
		sendFileToClient(clientSocket, msg, user);
		return;
	default:
		return;
	}
}

void getNameAndFiles(int clientSocket, User* user) {
	if (!user) {
		return;
	}
	int numOfFiles = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir(user->dir_path);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			numOfFiles++;
		}
		closedir(d);
	}
	char* arg = (char*) malloc(sizeof(char) * (MAX_USERNAME_SIZE + 30));
	sprintf(arg, "Hi %s, you have %d files stored.\n", user->user_name,
			numOfFiles);
	Message* msg = createServerMessage(LOGIN_DETAILS, arg, NULL);
	send_command(clientSocket, msg);
	free(arg);
	free(msg);
	return;
}

/*
 * welcome the accepted socket, get username and password and checks if it fits
 * any user name in the users list *users. if not, allows client to quit or retype,
 * else, calls getNameAndFiles to greet the user.
 *
 */

int client_serving(int clientSocket, User *users, int numOfUsers) {
	User* user = NULL;
	Message *msg = (Message *) malloc(sizeof(Message) + 1);
	receive_command(clientSocket, msg);
	if (msg->type == LOGIN_DETAILS) {
		for (int i = 0; i < numOfUsers; i++) {
			if (strcmp(users[i].user_name, msg->arg1) == 0) {
				if (strcmp(users[i].password, msg->arg2) == 0) {
					user = &users[i];
					getNameAndFiles(clientSocket, user);
				}
			}
		}
		if (user == NULL) {
			char* command = "WRONG";
			msg->arg1 = command;
			send_command( clientSocket,msg);

		}
	}
	while (msg->type != QUIT) {
		receive_command(clientSocket, msg);
		handleMessage(clientSocket, msg, user);
	}
	return 0;
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

	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	my_addr.sin_family = AF_INET;
	status = bind(socketfd, (struct sockaddr *)&my_addr, sizeof(my_addr));

	if (status < 0) {
		printf("Error type: %d\n", errno);
		return;
	}
	if (listen(socketfd, 1) < 0) {
		printf("Could not listen...");
		return;
	}
	while (1) {
		newsocketfd = accept(socketfd,  (struct sockaddr *)&client_addr, &client_size);
		if (newsocketfd < 0) {
			printf("accept() not successful...");
			return;
		}
		client_serving(newsocketfd, usersArray, numOfUsers);
		close(newsocketfd);
	}
}

void start_server(char* users_file, const char* dir_path, int port) {
	//creating the main folder
	//reading input file
	FILE* usersFile = fopen(users_file, "r");
	User* usersArray = (User*) malloc(15 * sizeof(User));
	printf("salami1 \n");
	int numOfUsers = 0;
	if (usersFile != NULL) {
		char* user_buffer = (char*) malloc(sizeof(char*) * 26);
		char* pass_buffer = (char*) malloc(sizeof(char*) * 26);
		char* fileDirPath = (char*) malloc(sizeof(dir_path) + 1);
		while (fscanf(usersFile, "%s\t", user_buffer) > 0) {
			printf("%s \n", user_buffer);
			strcpy(fileDirPath, dir_path);
			strcat(fileDirPath, "/");
			strcat(fileDirPath, user_buffer);
			printf("file dir path: %s \n", fileDirPath);
			if (!mkdir(fileDirPath ,ACCESSPERMS)) {
				User* newUser = (User*) malloc(sizeof(User));
				newUser->user_name = user_buffer;
				printf("check this!!! \n");

				fscanf(usersFile, "%s\n", pass_buffer);
				printf("pass_buffer: %s\n", pass_buffer);
				newUser->password =pass_buffer;
				newUser->dir_path = fileDirPath;
				usersArray[numOfUsers] = *newUser;
				numOfUsers++;
				printf("newUSER\n");
				free(newUser);
			} else {
				printf("Error: %s \n",strerror(errno));
				printf("cannot create user directory for :%s\n", user_buffer);
			}
			user_buffer = (char*) malloc(
					sizeof(char*) * MAX_USERNAME_SIZE + 1 + strlen("User :"));
			pass_buffer = (char*) malloc(
					sizeof(char*) * MAX_PASSWORD_SIZE + 1 + strlen("Password"));

		}
		free(fileDirPath);
		free(user_buffer);
		free(pass_buffer);
		start_listen(usersArray, numOfUsers, port);
	}
}
