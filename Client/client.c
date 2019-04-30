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

void cut(char* input, char** k,int* nbarg){
	int l = strlen(input);
	int y = 0;
	*nbarg = 0;
	int j = 0;
	char tmp[100];
	memset(tmp, 0, 100);
	for(int i = 0 ; i < l ; i++){
		if(input[i] == ' ' || input[i] == '\0' || input[i] == 0 || input[i] == '\n'){
			printf("\n");
			printf("%s \n", tmp);
			strcpy(*(k + y), tmp);
			printf("%s \n", k[y]);
			y++;
			j=0;
			memset(tmp, 0, 100);
			*nbarg = *nbarg + 1;
		}
		else{
			tmp[j] = input[i];
			printf("%d ---> |%c| \n",tmp[j], tmp[j]);
			j++;
		}
	}
}


void analyse(){
	int sockfd = 0;
    char recvBuff[1024];
    char chat[1024];

	printf("Username ? : ");
	//fgets(chat, 1024, stdin);
	gets(chat);
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
	strcpy(buffer, msg);
	int u = send(*sockfd, buffer, 1024, 0);
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
						if(input[i] == '\0'){
							break;
						}
						continue;
					}
					if(hoeing == 1){
						strncpy(port, current, y);
						memset(current, 0, 40);
						hoeing++;
						y = 0;
						if(input[i] == '\0'){
							break;
						}
						continue;
					}
					if(hoeing == 2){
						strncpy(usrname, current, y-1);
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
			int a;
			if(hoeing == 3){
				printf("Connection as : %s on %s port : %s \n", usrname, adr, port);
				if(a = connectionServer(sockfd, adr, atoi(port), usrname) == -1){
					printf("Check if the adresse/port is good\n");
				}
			}
			else{
				printf("Usage: /connect <ip of server> <port> <Username> \n");
			}
			

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
	int y;
	if((y = recv(*sockfd, rec, 1024, 0)) > 0){
		printf("%s\n", rec);
	}
}

int main()
{
	int sockfd;
	char t[200];
	//connectionServer(&sockfd, "127.0.0.1", 5000, "Archoline");
	/*while(1){
		memset(t, '0', 200);
		fgets(t, 200, stdin);
		sentence(t, &sockfd);

	}*/
	char* p[100];
	int y = 0;
	char u[50];
	memset(u, 0, 50);
	fgets(u, 20, stdin);
	char m[50][10];
	for(int i = 0 ; i < 10 ; i++){
		memset(m[i], 0, 50);
	}
	
	cut(u,m, &y);
	for(int i = 0 ; i < 2 ; i++){
		//printf("%s",m[i]);
	}
	printf("%d\n", y);
    
}
