/*
 * client.c
 *
 *  Created on: 23 Oct 2014
 *      Author: abo278
 */
#include "client.h"

void loop_client() {
	int socket_fdesc, new_socket_fdesc;
	struct sockaddr_in client;
	struct sockaddr_in server;
	socklen_t n;
	socklen_t client_len;
	char buffer[BUFFER_SIZE];
	IplImage* image;

	IplImage buffer[];

	cvNamedWindow("Simulator", CV_WINDOW_AUTOSIZE);
	image = cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3); //Bild mit 320*240 Pixel á 3 Bytes

	if ((socket_fdesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		errorHandler("no socket connection established.");
	}
	puts("socket_fdesc created");

	client.sin_family = AF_INET;
	client.sin_port = htons(SIN_PORT);
	client.sin_addr.s_addr = htonl(SIN_ADDRESS);

	if (connect(socket_fdesc, (struct sockaddr *) &client, sizeof(server))
			== -1) {
		errorHandler("connection failed");
	}

	n = sizeof(struct sockaddr_in);
	if (getsockname(socket_fdesc, (struct sockaddr *) &server, &n) == -1) {
		errorHandler("getsocketname failed");
	}
	printf("socket: %x:%d\n", ntohl(server.sin_addr.s_addr),
			ntohs(server.sin_port));
	while (1) {
		if(recv(socket_fdesc,image->imageData,image->imageSize,MSG_WAITALL)==-1){
			perror("recv failed");
		}
		//Bild anzeigen
		cvShowImage("Simulator", image);
		//und auf Ereignisse des Betriebssystems warten (erst hier wird das Bild tatsaechlich aktualisiert
		if ((cvWaitKey(5) & 255) == 27) {
			break;
		}
	}
	if (close(socket_fdesc) == -1) {
		errorHandler("failed to close");
	}
	puts("socket closed");
}

