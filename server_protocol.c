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

void addFile(Message* msg, User* user) {
	if (!user || !msg){
		printf("Error in Message or User");
		return;
	}
	char pathToFile[sizeof(user->dir_path) + sizeof(msg->arg1) + 5];
	strcpy(pathToFile, user->dir_path);
	strcat(pathToFile, "/");
	strcat(pathToFile, msg->arg1);
	FILE *file = fopen(pathToFile, "w");
	Message* msgToSend;
	if (file == NULL){
		printf("File Not Added\n");
		msgToSend = createServerMessage(msg->type,"ERROR", NULL);
		free(msgToSend);
		free(pathToFile);
		return;
	}
	fwrite(msg->arg2, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	msg = createServerMessage(msg->type, "File added", NULL);
	/// send//
	free(msg);
	free(pathToFile);


}

void deleteFile(Message* msg, User* user) {
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
	Message* msgToSend = createServerMessage(DELETE_FILE, arg, NULL );
	/// send msgToSend ///
	free(arg);
	free(msgToSend);
}

void sendListOfFiles(User* user) {
	if (!user) {
		return;
	}
	char names[MAX_FILES_PER_CLIENT][MAX_FILE_NAME];
	DIR *d;
	struct dirent *dir;
	int i = 0;
	d = opendir(user->dir_path);
	if (d) {
		while ((dir = readdir(d)) != NULL ) {
			names[i] = dir->d_name;
			i++;
		}
		closedir(d);
	}
	Message* msg = createServerMessage(LIST_OF_FILES, names, NULL );
	/// send message////

	free(msg);
	return;
}

void handleMessage(Message msg, User* user) {
	if (!msg) {
		return;
	}
	switch (msg.type) {
	case LIST_OF_FILES:
		sendListOfFiles(user);
		return;
	case DELETE_FILE:
		deletFile(msg, user);
		return;
	case ADD_FILE:
		return;
	case GET_FILE:
		return;
	default:
		return;
	}
}

void getNameAndFiles(User* user) {
	if (!user) {
		return;
	}
	int numOfFiles = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir(user->dir_path);
	if (d) {
		while ((dir = readdir(d)) != NULL ) {
			numOfFiles++;
		}
		closedir(d);
	}
	char* arg = (char*) malloc(sizeof(char) * (MAX_USERNAME_SIZE + 30));
	sprintf(arg, "Hi %s, you have %d files stored.\n", user->user_name,
			numOfFiles);
	Message* msg = createServerMessage(LOGIN_DETAILS, arg, NULL );
	///////send message///////

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

int client_serving(int clientsocket, User *users, int numOfUsers) {
	printf("Welcome! Please log in.\n");
	User* user = NULL;
	int flag = 1;
	Message msg;
	msg = get_message();
	if (msg.type == LOGIN_DETAILS) {
		for (int i = 0; i < numOfUsers; i++) {
			if (strcmp(users[i]->user_name, msg.arg1) == 0) {
				if (strcmp(users[i]->password, msg.arg2) == 0) {
					user = users[i];
					getNameAndFiles(user);
				}
			}
		}
		if (user == NULL ) {
			printf("Wrong user name or password, please try again or quit\n");
		}
	}
	while (msg.type != QUIT) {
		msg = get_message;
		handleMessage(msg);
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
	//TODO:my_addr.sin_addr = ask jhonatan!!!!!!
	status = bind(socketfd, &my_addr, sizeof(my_addr));

	if (status < 0) {
		printf("Could not bind IP to socket\n");
		return;
	}

	if (listen(socketfd, 1) < 0) {
		printf("Could not listen...");
		return;
	}
	while (1) {
		newsocketfd = accept(socketfd, &client_addr, &client_size);
		if (newsocketfd < 0) {
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

	if (usersFile != NULL ) {
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
		start_listen(usersArray);
	}
}
