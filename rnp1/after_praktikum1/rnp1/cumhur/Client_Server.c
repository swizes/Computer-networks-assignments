#include "client_server.h"

#define MAIN_VALID_ARGC 3
#define LISENQ 20
#define SERVERPORT 5001
#define CLIENTSIZE 3
#define ESCAPE_KEY_INT 27
#define CV_CREATE_DEFAULT_IMAGE cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3)
#define BUFFER_FRAME_SIZE 20
#define IMAGE_SIZE 230400

IplImage* img;
int is_showImage_ready;
int sock, sockServ;
struct in_addr server_ip;
int server_port;
int count = 0;
sem_t position;
CvSize size;
char buffer[BUFFER_FRAME_SIZE][IMAGE_SIZE];
int bufferWritePosition = 0;
IplImage * readImage;

int key;

pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condNewImage = PTHREAD_COND_INITIALIZER;
pthread_t *new_thread;

void writeBuffer(IplImage *);
char * readBuffer();

void writeBuffer(IplImage * img) {
	pthread_mutex_lock(&mutexBuffer);
	bufferWritePosition = (bufferWritePosition + 1) % BUFFER_FRAME_SIZE;
//	buffer[bufferWritePosition] = img->imageData;
	is_showImage_ready = 1;
	//TODO cond_newImage = true;
	pthread_cond_broadcast(&condNewImage);
	pthread_mutex_unlock(&mutexBuffer);
}

char * readBuffer() {
	char * result;
	pthread_mutex_lock(&mutexBuffer);
	//TODO: wait on condition
	pthread_cond_wait(&condNewImage);
	result = buffer[bufferWritePosition];
	pthread_mutex_unlock(&mutexBuffer);
	return result;
}

int main(int argc, char** argv) {

	pthread_t thread_c, thread_s;
	if (argc != MAIN_VALID_ARGC) {
		quit("Usage: stream_client <server_ip> <server_port>", 0);
	}

	struct addrinfo hints;
	struct addrinfo *res = 0;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if ((getaddrinfo(argv[1], argv[2], &hints, &res)) < 0) {
		quit("failed to get address info", 0);
	}
	/* get the parameters */
	server_ip = ((struct sockaddr_in *) res->ai_addr)->sin_addr;
	server_port = atoi(argv[2]);

	/* create image */
	img = CV_CREATE_DEFAULT_IMAGE;

	//TODO: create buffer

	/* run the streaming client as a separate thread */
	if (pthread_create(&thread_c, NULL, streamClient, NULL)) {
		quit("pthread_create client failed.", 1);
	}

//	if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
//		quit("pthread_create server failed.", 1);
//	}
	showImage();

	/* user has pressed 'q', terminate the streaming client */
	int thread_c_return, thread_s_return;
	pthread_join(thread_c, (void *) &thread_c_return);
//	pthread_join(thread_s, (void *) &thread_s_return);

// TODO release buffer and possibly other
	quit("", thread_c_return - thread_s_return);
	return 0;

}
void showImage(void) {
	cvNamedWindow("stream_client", CV_WINDOW_AUTOSIZE);

	fprintf(stdout, "Press 'q' to quit.\n\n");
	while (key != 'q') {//TODO: correct q
		key = cvWaitKey(5) & 255;

		pthread_mutex_lock(&mutexBuffer);
		if (is_showImage_ready) {
			cvShowImage("stream_client", img);
			is_showImage_ready = 0;
		}
		pthread_mutex_unlock(&mutexBuffer);
		if (key == ESCAPE_KEY_INT) {
			break;
		}
	}

	/* free memory */
	cvDestroyWindow("stream_client");

}

/**
 *  This is the streaming client, run as separate thread
 */

void* streamClient(void* arg) {

	struct sockaddr_in client;

	/* create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		quit("socket() failed.", 1);
	}

	/* setup server parameters */
	memset(&client, 0, sizeof(client));

	client.sin_family = AF_INET;
	client.sin_addr.s_addr =server_ip.s_addr;
	client.sin_port = htons(server_port);
	printf("ip: %d, port: %d\n", (int)server_ip.s_addr, server_port);
	fflush(stdout);

	/* connect to server */
	if (connect(sock, (struct sockaddr*) &client, sizeof(client)) < 0) {
		quit("connect() failed.", 1);
	}
//	int imgsize = img->imageSize;
//	char sockdata[imgsize];
//	int i, j, k, bytes;

	/* start receiving images */
	while (1) {
//		for (i = 0; i < imgsize; i += bytes) {
//			if ((bytes = recv(sock, sockdata + i, imgsize - i, 0)) <= 0) {
//				quit("recv failed", 1);
//			}
//
//		}
//		erzeuger_loop(sockdata);
//		pthread_mutex_lock(&mutex);
		/* convert the received data to OpenCV's IplImage format, thread safe */
//		for (i = 0, k = 0; i < img->height; i++) {
//			for (j = 0; j < img->width * 3; j++) {
//				((char*) (img->imageData + i * img->widthStep))[j] =
//						sockdata[k++];
//			}
//		}
//		is_data_ready = 1;
//		pthread_mutex_unlock(&mutexBuffer);
		/* get raw data */
		if (recv(sock, img->imageData, img->imageSize, MSG_WAITALL) == -1) {
			perror("client: recv failed");
		}
		writeBuffer(img);

		/* possibility to wait to reduce frame rate */
		//usleep(1000);
	}

	return 0;

}
/*###########################################################################################################################*/

void* streamServer(void* arg) {

	struct sockaddr_in server;
	sem_init(&position, 0, CLIENTSIZE);
	/* open socket */
	if ((sockServ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		quit("socket() failed", 1);
	}

	/* allocate servers socket address memory */
	bzero((char *) &server, sizeof(server));
	memset(&server, 0, sizeof(server));

	/* setup server's IP and port */
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVERPORT);
	server.sin_addr.s_addr = INADDR_ANY;

	/* bind the socket */
	if (bind(sockServ, (struct sockaddr*) &server, sizeof(server)) == -1) {
		quit("bind() failed", 1);
	}

	/* wait for connection */
	if (listen(sockServ, CLIENTSIZE) == -1) {
		quit("listen() failed.", 1);
	}

	/* accept a client */
	int clientfd;
	struct sockaddr_in client_addr;

	socklen_t clientLen;
	bzero(&client_addr, sizeof(client_addr));

	/*---accept a connection (creating a data pipe)---*/
	while (1) {

		sem_wait(&position);
		printf("waiting for connections..........\n");

		clientfd = accept(sockServ, (struct sockaddr*) &client_addr,
				&clientLen);
		if (clientfd < 0) {
			perror("accept error\n");
		}
		printf("%s:%d connected on %d\n", inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port), clientfd);
		new_thread = malloc(sizeof(pthread_t));
		pthread_create(new_thread, NULL, clientexternOperation,
				(void *) &clientfd);
		pthread_detach(*new_thread);
	}
	/* close the server socket */
	close(sockServ);
	return 0;

}
void * clientexternOperation(void* sockfd) {
	int clientfd = *((int *) sockfd);
	int bytes;
	IplImage readImage = CV_CREATE_DEFAULT_IMAGE;

	while (1) {
		readImage->imageSize = IMAGE_SIZE;
		readImage->imageData = readBuffer();
		bytes = send(clientfd, readImage->imageData, readImage->imageSize,
		MSG_NOSIGNAL);
		if (bytes < 0) {
			printf("broken pipe on fd: " + clientfd);
			break;
		}
	}
	//TODO destroy image
	close(clientfd);
	sem_post(&position);
	return 0;
}
/**
 * This function provides a way to exit nicely from the system
 */
void quit(char* msg, int retval) {
	if (retval == 0) {
		fprintf(stdout, (msg == NULL ? "" : msg), msg);
		fprintf(stdout, "\n");
	} else {
		fprintf(stderr, (msg == NULL ? "" : msg), msg);
		fprintf(stderr, "\n");
	}
	if (sock) {
		close(sock);
		close(sockServ);

		printf("server ended.\n");
	}

	if (img)
		cvReleaseImage(&img);
	free(new_thread);

	pthread_mutex_destroy(&mutexBuffer);
	exit(retval);
}
