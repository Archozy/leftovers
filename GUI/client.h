#ifndef PIXEL_OPERATIONS_H_
#define PIXEL_OPERATIONS_H_

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>
#include <signal.h>

void analyse();
int connectionServer(int* sockfd, char* adresse, int port, char* Username);
int sendMSG(int* sockfd, char* msg);
void sentence(char* input, int* sockfd);
int recvMSG(int *sockfd);

#endif
