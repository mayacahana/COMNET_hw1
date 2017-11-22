all:
	gcc -Wall -g -o  server file_server.h file_server.c server_protocol.h server_protocol.c
	gcc -Wall -g -o  client file_client.h file_client.c client_protocol.h client_protocol.c
clean:
	@rm -f $(PROGRAMS) *.o server
	@rm -f $(PROGRAMS) *.o client