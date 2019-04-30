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
#include <sys/sendfile.h>
#include "encryption.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#define MAX_BUF 1024
int count = 0; 
void analyse(){
	int sockfd = 0;
    char recvBuff[1024];
    char chat[1024];

	printf("Username ? : ");
	//fgets(chat, 1024, stdin);
    //gets(chat);
    memset(recvBuff, '0',sizeof(recvBuff));
    
    strcat(chat, "\0");
    
    int y;
    char rec[1024];
    while(1){
		if((y = recv(sockfd, rec, 1024, 0)) > 0){
			printf("%s\n", rec);
		}
		fgets(chat, 1024, stdin);
		int u = send(sockfd, chat, 1024, 0);
		if(u < 0){
			printf("EROROR server timed out disconnexion\n");
		}
		memset(chat, '0',1024);
	}
}

int sendFile(int *sock, char* pic){
	FILE *fp = fopen(pic, "r");
	if (fp == NULL)
        return -1;
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char bs[size + 1024];
	memset(bs, 0, 1024);
	
	printf("converted : %s\n", bs);
	int b;
	if(fp < 0){
		exit(0);
	}
	
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
	sprintf(bs, "/8%d|%s#", size, pic);
    while ((read = getline(&line, &len, fp)) != -1) {
        strcat(bs, line);
        free(line);
        line = NULL;
    }
	
	printf("Sent size \n");
	
    printf("Sending Picture as Byte Array\n");
	
	printf("Sending : %s\n", bs);
	int y = send(*sock, bs, 1024, 0);
	if(y < 0)
		return 1;
	fclose(fp);
	
    return 0;



	
	
	
}




int sendPicture(int *sock, char* pic){
	FILE *fp = fopen(pic, "r");
	if (fp == NULL)
        return -1;
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char bs[size + 1024];
	memset(bs, 0, 1024);
	
	sprintf(bs, "/9%d|%s#", size, pic);
	write(*sock, bs, 1024);
	printf("converted : %s\n", bs);
	int b;
	if(fp < 0){
		return 0;
	}


	//Send Picture as Byte Array
	printf("Sending Picture as Byte Array\n");
	char send_buffer[size];
	memset(send_buffer, 0, size);
	while(!feof(fp)) {
		fread(send_buffer, 1, sizeof(send_buffer), fp);
		write(*sock, send_buffer, sizeof(send_buffer));
		bzero(send_buffer, sizeof(send_buffer));
	}
	
    return 0;



	
	
	
}
int connectionServer(int* sockfd, char* adresse, int port, char* Username){
	struct sockaddr_in serv_addr;
	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return -1;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 

    if(inet_pton(AF_INET, adresse, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return -1;
    } 

    if(connect(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       sleep(1);
       return -1;
    }
    fcntl(*sockfd, F_SETFL, SOCK_NONBLOCK);
    send(*sockfd, Username, 1024, 0);
    return 0;
}
int sendMSG(int* sockfd, char* msg){
	char buffer[1024];
	
	memset(buffer, 0, 1024);
	encrypt(msg, 1);
	strcat(buffer, "/1");
	strcat(buffer, msg);
	printf("Sending %s\n", buffer);
	int u = send(*sockfd, buffer, 1024, 0);
	if(u < 0){
		printf("not connected to any Server please type </help connect>\n");
		return -1;
	}
	return 0;
}
int askList(int* sockfd){
	char buffer[5];
	memset(buffer, 0, 5);
	strcat(buffer, "/2");
	printf("Sending %s\n", buffer);
	int u = send(*sockfd, buffer, 5, 0);
	if(u < 0){
		printf("not connected to any Server please type </help connect>\n");
		return -1;
	}
	return 0;
}
void sentence(char* input, int* sockfd){
	if(strncmp(input, "/", 1) == 0){
		if(strncmp(input, "/help", 5) == 0 && strlen(input) == 6){
			printf("type /help <command> \n");
		}
		if(strncmp(input, "/quit", 5) == 0){
			exit(0);
		}
		if(strncmp(input, "/sendPic", 8) == 0){
			char picname[20];
			memset(picname, 0, 20);
			for(int i = 9 ; input[i] != '\0' ; i++){
				picname[i - 9] = input[i];
			}
			printf("Sending %s \n", picname);
			
			//send(*sockfd, bud, 2, 0);
			sendPicture(sockfd, picname);
		}
		
		if(strncmp(input, "/sendFile", 9) == 0){
			char picname[20];
			memset(picname, 0, 20);
			for(int i = 10 ; input[i] != '\0' ; i++){
				picname[i - 10] = input[i];
			}
			printf("Sending %s \n", picname);
			
			//send(*sockfd, bud, 2, 0);
			sendFile(sockfd, picname);
		}
		
		if(strncmp(input, "/disconnect", 11) == 0){
			shutdown(*sockfd, 2);
		}
		if(strncmp(input, "/list", 5) == 0){
			askList(sockfd);
		}
		if(strncmp(input, "/help connect", 13) == 0){
			printf("Usage: /connect <ip of server> <port> <Username> \n");
		}
		if(strncmp(input, "/connect", 8) == 0){
			int i = 8;
			int y = 0;
			int hoeing = 0;
			char current[40];
			memset(current, 0, 40);
			char adr[20];
			char port[5];
			char usrname[20];
			memset(adr, 0, 20);
			memset(port, 0, 5);
			memset(usrname, 0, 20);
			for(i = 9 ; 1; i++){
				if(input[i] == ' ' || input[i] == '\0' ){
					current[y] = '\0';
					
					if(hoeing == 0){
						strncpy(adr, current, y);
						memset(current, 0, 40);
						hoeing++;
						y = 0;
						continue;
					}
					if(hoeing == 1){
						strncpy(port, current, y);
						memset(current, 0, 40);
						hoeing++;
						y = 0;
						continue;
					}
					if(hoeing == 2){
						strncpy(usrname, current, y);
						hoeing++;
						memset(current, 0, 40);
						break;
					}
				}
				else{
				
					current[y] = input[i];
					y++;
				}
			}
			if(hoeing < 2){
				return;
			}
			printf("Connection as : %s on %s port : %s \n", usrname, adr, port);
			connectionServer(sockfd, adr, atoi(port), usrname);

		}
		
	}
	else{
		if(input[0] != '\n'){
			sendMSG(sockfd, input);
		}
		
	}
}


int recvMSG(int *sockfd){
	char rec[1024];
	char buff[1024];
	int y;
	char msg[1024];
	char usr[30];
	char inst[2];
	int u = 0;
	memset(rec, 0, 1024);
	memset(msg, 0, 1024);
	memset(buff, 0, 1024);
	memset(usr, 0, 30);
	y = recv(*sockfd, rec, 1024, 0);
	if(y < 0){
		return -1;
	}
	printf("Brute -- %s\n", rec);
	if(inst[1] == '1'){
		printf("size (%d) \n", y);
		int w = 0;
		for(int i = 0 ; rec[i] != 0; i++){
			printf("<%c>\n", rec[i]);
			if(rec[i] == ':'){
				w = i;
				continue;
			}
			if(!w){
				usr[i] = rec[i];
			}
			else{
				msg[i - w - 1] = rec[i]; 
			}
		}
		decrypt(msg, 1);
		printf("<%s>%s\n", usr, msg);
	}
	
}

/*int main()
{
	int sockfd;
	char t[200];
	//connectionServer(&sockfd, "127.0.0.1", 5000, "Archoline");
	while(1){
		memset(t, '0', 200);
		fgets(t, 200, stdin);
		sentence(t, &sockfd);

	}
    
}*/
