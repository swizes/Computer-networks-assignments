/*
 * main.h
 *
 *  Created on: 23 Oct 2014
 *      Author: abo278
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


#define SIN_PORT 5000
#define SIN_ADDRESS 0x7F000001
#define BUFFER_SIZE 256

/* FUNCTIONS */
/* SERVER */
void loop_server();

/* CLIENT */
void loop_client();

void errorHandler(char * errMessage);
#endif /* MAIN_H_ */
