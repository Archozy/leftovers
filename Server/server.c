#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#define MAX_BUF 1024
int count = 0; 
struct vector{
	int size;
	struct client* data;
	
};

struct client{
	int connfd;
	char* username;
	int renamed;
};

void init_client(struct client* cli){
	cli->connfd = 0;
	cli->username = calloc(50, sizeof(char));
	cli->renamed = 0;
}
void client_free(struct client* cli){
	free(cli->username);
}
void vector_init(struct vector* vect){
	vect->data = malloc(sizeof(struct client));
	vect->size = 0;
}
void vector_add(struct vector* vect, struct client e){
	vect->data[vect->size] = e;
	vect->size++;
	vect->data = realloc(vect->data, sizeof(struct client) * (vect->size + 1));
}
int vector_search(struct vector* vect, int e){
	for(int i = 0 ; i < vect->size ; i++){
		if(vect->data[i].connfd == e){
			return 1;
		}
	}
	return 0;
}
int vector_remove(struct vector* vect, int e){
	if(e < 0 || e >= vect->size){
		return 0;
	}
	for(int i = 0 ; i < vect->size ; i++){
		if(i >= e && i < vect->size - 1){
			vect->data[i] = vect->data[i+1];
		}
	}
	vect->size--;
	return 1;
}
void vector_print(struct vector* vect){
	printf("taille = %d\n", vect->size);
	for(int i = 0 ; i < vect->size ; i++){
		printf("Client %d {Socket : %d ---> username : %s}\n",i, vect->data[i].connfd, vect->data[i].username);
	}
}

int direxist(char* usr){
	
	
	char tui[50];
	memset(tui, 0, 50);
	if(usr[0] < 10){
		for(int i = 1 ; usr[i] != '\0' ; i++){
			tui[i - 1] = usr[i];
		}
	}
	else{
		strcpy(tui, usr);
	}
	for(int i = 0 ; tui[i] != '\0' ; i++){
		printf("|%d|", tui[i]);
	}
	DIR* dir = opendir(tui);
	if (dir)
	{
		/* Directory exists. */
		printf("ALready exists \n");
		closedir(dir);
	}
	else if (ENOENT == errno)
	{
		/* Directory does not exist. */
		printf("Creating a directory for %s\n", tui);
		mkdir(tui, 0777);
	}
	else
	{
		/* opendir() failed for some other reason. */
		printf("Error Dir \n");
	}
}

int recvPic(int *new_sock){
	//Read Picture Size
	printf("Reading Picture Size\n");
	int size = 0;
	int bs[1];
	printf("Reading Picture Size\n");
	int y = recv(*new_sock, bs, 1, 0);
	if(y < 0){
		printf("Crashed");
		exit(0);
	}
	printf("Size %d\n", size);
	//Read Picture Byte Array
	printf("Reading Picture Byte Array\n");
	char p_array[size];
	read(*new_sock, p_array, size);

	//Convert it Back into Picture
	printf("Converting Byte Array to Picture\n");
	FILE *image;
	image = fopen("c1.png", "w");
	fwrite(p_array, 1, sizeof(p_array), image);
	fclose(image);
}
int main(int argc, char *argv[])
{
	
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
	
    //int j = sprintf(sendBuff, "Test Message Server \n");ù
	printf("Server Launched Port : %s \n", argv[1]);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listenfd, F_SETFL, SOCK_NONBLOCK);
    memset(&serv_addr, '0', sizeof(serv_addr));
    int port = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 
    int nb = 0;
    char buf[1024];
    memset(buf, '§', 1024);
	struct vector vect;
	int u = 0;
	vector_init(&vect);/*
	struct client cz = {4, "USER", 0};
	struct client cA = {5, "USER2", 0};
	struct client cB = {6, "USER3", 0};
	struct client cC = {7, "USER4", 0};
	vector_add(&vect, cz);
	vector_add(&vect, cA);
	vector_add(&vect, cB);
	vector_add(&vect, cC);
	vector_print(&vect);
	vector_remove(&vect, 3);
	vector_print(&vect);*/
    while(1)
    {
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		fcntl(connfd, F_SETFL, SOCK_NONBLOCK);
		if(!vector_search(&vect, connfd) && connfd != -1){
			struct client cli;
			init_client(&cli);
			cli.renamed = 0;
			cli.connfd = connfd;
			printf("Number %d connecting... \n", connfd);
			vector_add(&vect, cli);
			printf("List of connected : \n");
			//fcntl(vect.data[vect.size - 1].connfd, F_SETFL, SOCK_NONBLOCK);
			for(int i = 0; i < vect.size ; i++){
				if(vect.data[i].connfd == connfd){
					printf("---> YOU (cute) (%s): connected\n", vect.data[i].username);
				}else{
					printf("---> %s : connected\n", vect.data[i].username);
				}
			}
		}
		
		
		
		for(int i = 0; i < vect.size ; i++){
			int y = recv(vect.data[i].connfd, buf, 1024, 0);
			
			if(y > 0){
				if(vect.data[i].renamed == 0){
					for(int j = 0 ; j < 1024 ; j++){
						vect.data[i].username[j] = buf[j];
						if(buf[j] == '\0'){
							char chem[50];
							//strcat(chem, "User/");
							strcat(chem, buf);
							direxist(chem);
							break;
						}
						
					}
					vect.data[i].renamed = 1;
					//printf("Username changed in %s \n", vect.data[i].username);
					memset(buf, '0', 1024);
				}
				else{
					if(buf[0] == '/' && buf[1] == '9'){
						printf("How a picture Nice \n");
						
						printf("%s is sending a file ...\n", vect.data[i].username);
						//Read Picture Size
						
						
						char ts[50];
						char name[50];
						memset(ts, 0, 50);
						memset(name, 0, 50);
						int change = 0;
						int cle = 0;
						printf("Why does it take so much times \n");
						int ofs = 0;
						for(int y = 2 ; buf[y] != '#' ; y++){
							if(change == 0 && buf[y] != '|'){
								ts[y - 2] = buf[y];			
							}
							if(change == 1){
								name[y - cle] = buf[y];					
							}
							if(buf[y] == '|'){
								change++;
								cle = y+1;
							}
							ofs = y;
							
						}
						
						int size = atoi(ts);
						printf("Size : %s \n", ts);
						printf("Size : %d \n", size);
						printf("Reading Picture Size\n");
						char buff[1024];
						memset(buff, 0, 1024);
						strcat(buff, vect.data[i].username);
						strcat(buff, "/");
						strcat(buff, name);
						

						//Read Picture Byte Array
						printf("Reading Picture Byte Array\n");
						char p_array[size];
						read(vect.data[i].connfd, p_array, size);

						//Convert it Back into Picture
						printf("Converting Byte Array to Picture\n");
						FILE *image;
						printf("%s\n", buff);
						image = fopen(buff, "w");
						if(image == NULL){
							printf("BUG");
						}
						else{
							fwrite(p_array, 1, sizeof(p_array), image);
							7fclose(image);
						}
						
						
					}
					
					if(buf[0] == '/' && buf[1] == '8'){
						printf("AWWW a file......Nice \n");

						
						long size = 0;
						
						printf("%s is sending a file ...\n", vect.data[i].username);
						//Read Picture Size
						
						
						char ts[50];
						char name[50];
						memset(ts, 0, 50);
						memset(name, 0, 50);
						int change = 0;
						int cle = 0;
						printf("Why does it take so much times \n");
						int ofs = 0;
						for(int y = 2 ; buf[y] != '#' ; y++){
							if(change == 0 && buf[y] != '|'){
								ts[y - 2] = buf[y];			
							}
							if(change == 1){
								name[y - cle] = buf[y];					
							}
							if(buf[y] == '|'){
								change++;
								cle = y+1;
							}
							ofs = y;
							
						}
						size = atoi(ts);
						printf("Size : %s \n", ts);
						printf("Size : %d \n", size);
						char p_array[size];
						memset(p_array, 0, size);
						for(int y = ofs + 2 ; buf[y] != '\0' ; y++ ){
							p_array[y - (ofs + 2)] = buf[y];
						}
						printf("file : \n %s \n", p_array);
						char buff[1024];
						int b = 0;
						int tot;
						memset(buff, 0, 1024);
						size = atoi(ts);
						
						char pou[70];
						
						FILE *image = NULL;
						printf("%s is sending a file ...\n", vect.data[i].username);
						strcat(buff, vect.data[i].username);
						strcat(buff, "/");
						strcat(buff, name);
						
					
						printf("%s\n", buff);
						image = fopen(buff, "w");
						if(image == NULL){
							printf("ERROR \n");
							
						}
						fprintf(image, "%s", p_array);
						printf("File successfully Received!\n");
						fclose(image);
						
					}
					
					
					
					
					if(buf[0] == '/' && buf[1] == '2'){
						char connected[1024];
						memset(connected, 0, 1024);
						strcat(connected, "/2");
						for(int j = 0 ; j < vect.size ; j++){
							
							strcat(connected,vect.data[j].username);
							strcat(connected, "\n");
						}
						int u = send(vect.data[i].connfd, connected, 1024, NULL);
						if(u < 0){
							printf("Error\n");
						}
					}
					if(buf[0] == '/' && buf[1] == '1'){
						
						printf("Message client reçus \n");
						char* lop = malloc(sizeof(char) * 1075);
						char tuf[1024];
						memset(tuf, 0, 1024);
						for(int i = 2 ; i < strlen(buf) ; i++){
							tuf[i - 2] = buf[i];
						}
						snprintf(lop,1050, "[%s]:%s\n", vect.data[i].username, tuf);
						printf("%s", lop);
						for(int k = 0; k < vect.size ; k++){
							memset(tuf, 0, 1024);
							strcat(tuf, "/1");
							strcat(tuf, lop);
							int u = send(vect.data[k].connfd, tuf, 1024, NULL);
							if(u < 0){
								printf("Error\n");
							}
							
						}
						free(lop);
						memset(buf, '0', 1024);
					}
					
					
				}
				
			}
			if(y == 0){
				printf("deconnexion from %s\n", vect.data[i].username);
				
				close(vect.data[i].connfd);
				client_free(&vect.data[i]);
				vector_remove(&vect, i);
			}
			memset(buf, '0', 1024);
		}
		
		

		
        
    }
}
