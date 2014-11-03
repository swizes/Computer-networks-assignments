#include "client_server.h"

#define MAIN_VALID_ARGC 3
#define LISENQ 20
#define SERVERPORT 5001
#define CLIENTSIZE 3
#define ESCAPE_KEY_INT 27
#define CV_CREATE_DEFAULT_IMAGE cvCreateImage(cvSize(320, 240), IPL_DEPTH_8U, 3)
#define BUFFER_FRAME_SIZE 40
#define IMAGE_SIZE 230400
#define MAX_SOCKET_PORT_TRIES 100

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
int exit_server = 1;
int exit_client = 1;

int key;

pthread_mutex_t mutexBuffer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condNewImage = PTHREAD_COND_INITIALIZER;
pthread_t *new_thread;

void writeBuffer(IplImage *);
char * readBuffer();
void rcvSignal(int sig);

char * readBuffer() {
    char * result;
    pthread_mutex_lock(&mutexBuffer);
    //TODO: wait on condition
    pthread_cond_wait(&condNewImage, &mutexBuffer);
    int bufferWritePosition_own = (bufferWritePosition + BUFFER_FRAME_SIZE / 2)
            % BUFFER_FRAME_SIZE;
    pthread_mutex_unlock(&mutexBuffer);
    //printf("reading\n");
    //destination = buffer[bufferWritePosition];
//    strcpy(destination, buffer[bufferWritePosition]);
    return buffer[bufferWritePosition_own];
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

    //TODO: create buffer
    memset(&buffer, 0, sizeof(char) * IMAGE_SIZE);

    /* run the streaming client as a separate thread */
    if (pthread_create(&thread_c, NULL, streamClient, NULL)) {
        quit("pthread_create client failed.", 1);
    }

    if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
        quit("pthread_create server failed.", 1);
    }
    showImage();

    /* user has pressed 'q', terminate the streaming client */
    exit_client = 0;
    exit_server = 0;
    pthread_join(thread_c, NULL);
    printf("client joined\n");
    pthread_kill(thread_s, SIGUSR1); // send a signal to the server to end it
    pthread_join(thread_s, NULL);
    printf("server joined\n");

// TODO release buffer and possibly other
    quit("bye", 0);
    return 0;

}
void showImage(void) {
    IplImage * img = CV_CREATE_DEFAULT_IMAGE;
    cvNamedWindow("stream_client", CV_WINDOW_AUTOSIZE);

    fprintf(stdout, "Press 'q' to quit.\n\n");
    while (key != 'q') { //TODO: correct q
        key = cvWaitKey(5) & 255;

        pthread_mutex_lock(&mutexBuffer);
        if (is_showImage_ready) {
            img->imageSize = IMAGE_SIZE;
            img->imageData = buffer[bufferWritePosition];
            cvShowImage("stream_client", img);
            is_showImage_ready = 0;
        }
        pthread_mutex_unlock(&mutexBuffer);
        if ((cvWaitKey(5) & 255) == ESCAPE_KEY_INT) {
            printf("got escape\n");
            break;
        }
    }
//    printf("destroying window\n");
//    /* free memory */
    cvDestroyWindow("stream_client");
    printf("destroyed window\n");

}

void writeBuffer(IplImage * img) {
    pthread_mutex_lock(&mutexBuffer);
    bufferWritePosition = (bufferWritePosition + 1) % BUFFER_FRAME_SIZE;
    strcpy(buffer[bufferWritePosition], img->imageData);
    is_showImage_ready = 1;
    //TODO cond_newImage = true;
    pthread_cond_broadcast(&condNewImage);
    pthread_mutex_unlock(&mutexBuffer);
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
    client.sin_addr.s_addr = server_ip.s_addr;
    client.sin_port = htons(server_port);
    printf("ip: %d, port: %d\n", (int) server_ip.s_addr, server_port);
    fflush(stdout);

    /* connect to server */
    if (connect(sock, (struct sockaddr*) &client, sizeof(client)) < 0) {
        quit("connect() failed.", 1);
    }
//	int imgsize = img->imageSize;
//	char sockdata[imgsize];
//	int i, j, k, bytes;

    /* create image */
    img = CV_CREATE_DEFAULT_IMAGE;

    /* start receiving images */
    while (exit_client) {
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
        pthread_mutex_lock(&mutexBuffer);
        bufferWritePosition = (bufferWritePosition + 1) % BUFFER_FRAME_SIZE;
        pthread_mutex_unlock(&mutexBuffer);
        //strcpy(buffer[bufferWritePosition], img->imageData);
        if (recv(sock, buffer[bufferWritePosition], img->imageSize, MSG_WAITALL)
                == -1) {
            perror("client: recv failed");
        }
        pthread_mutex_lock(&mutexBuffer);
        is_showImage_ready = 1;
        //TODO cond_newImage = true;
        pthread_cond_broadcast(&condNewImage);
        pthread_mutex_unlock(&mutexBuffer);
        //writeBuffer(img);

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
    server.sin_addr.s_addr = INADDR_ANY;
    int server_port_try = server_port;
    int tries = 0;
    do {
        tries++;
        server_port_try++;
        server.sin_port = htons(server_port_try);
        /* bind the socket */
    } while ((bind(sockServ, (struct sockaddr*) &server, sizeof(server)) == -1)
            && (tries < MAX_SOCKET_PORT_TRIES));

    /* wait for connection */
    if (listen(sockServ, CLIENTSIZE) == -1) {
        quit("listen() failed.", 1);
    }

    struct sigaction sigact;
    /* Setup signal handler */
    /* Handler for USR1 */
    sigact.sa_handler = rcvSignal;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if (sigaction(SIGUSR1, &sigact, NULL) == -1) {
        perror("Error installing signal handler for USR1");
        exit(EXIT_FAILURE);
    }

    /* accept a client */
    int clientfd;
    struct sockaddr_in client_addr;

    socklen_t clientLen;
    bzero(&client_addr, sizeof(client_addr));

    /*---accept a connection (creating a data pipe)---*/
    while (exit_server) {

        sem_wait(&position);
        printf("waiting for connections..........\n");

        clientfd = accept(sockServ, (struct sockaddr*) &client_addr,
                &clientLen);
        if (clientfd < 0) {
            perror("accepted no client\n");
        }
        printf("%s:%d connected on fd:%d\n", inet_ntoa(client_addr.sin_addr),
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

void rcvSignal(int sig) {
}

void * clientexternOperation(void* sockfd) {
    int clientfd = *((int *) sockfd);
    int bytes;
    IplImage * readImage = CV_CREATE_DEFAULT_IMAGE;

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
