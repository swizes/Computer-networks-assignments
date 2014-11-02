/*
 * server.c
 *
 *  Created on: 23 Oct 2014
 *      Author: abo278
 */
#include "server.h"

void loop_server() {
	int socket_fdesc, new_socket_fdesc;
	struct sockaddr_in client;
	struct sockaddr_in server;
	int n;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];

	if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		errorHandler("no socket connection established.");
	}
	puts("socket_fdesc created");

	server.sin_family = AF_INET;
	server.sin_port = htons(SIN_PORT);
	server.sin_addr.s_addr = htonl(SIN_ADDRESS);
	if ((bind(socket_fdesc, (struct sockaddr *) &server, sizeof(server)))
			== -1) {
		errorHandler("binding not successful");
	}
	puts("binding successful");

	listen(socket_fdesc, SERVER_QUEUE_LEN);
	while (SERVER_LOOP_COND) {
		client_len = sizeof(client);
		if ((new_socket_fdesc = accept(socket_fdesc,
				(struct sockaddr *) &client, &client_len)) == -1) {
			errorHandler("accept declined.");
		}
		puts("accepted");
		bzero(buffer, BUFFER_SIZE);
		if ((n = read(new_socket_fdesc, buffer, BUFFER_SIZE - 1))
				== -1) {
			errorHandler("could not read" + n);
		}

		printf("message: %s\n", buffer);

		if ((n = write(new_socket_fdesc, "got it", 6)) == -1) {
			errorHandler("could not write" + n);
		}
		close(new_socket_fdesc);

	}
	close(socket_fdesc);
}
