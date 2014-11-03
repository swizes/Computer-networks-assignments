

#ifndef CLIENT_SERVER_H_
#define CLIENT_SERVER_H_


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <semaphore.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>

//#include <asm/atomic.h> // for atomic operations on the bufferPointer


pthread_mutex_t mutexBuffer;

void* streamServer(void* arg);
void* streamClient(void* arg);
void showImage(void );
void quit(char* msg, int retval);


void * clientexternOperation(void* sockfd);


#endif /* CLIENT_SERVER_H_ */
