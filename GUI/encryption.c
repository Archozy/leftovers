#include "encryption.h"

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

void encrypt(char password[],int key)
{
    char ch;
	int i;
	
	for(i = 0; password[i] != '\0'; ++i){
		ch = password[i];
		
		if(ch >= 'a' && ch <= 'z'){
			ch = ch + key;
			
			if(ch > 'z'){
				ch = ch - 'z' + 'a' - 1;
			}
			
			password[i] = ch;
		}
		else if(ch >= 'A' && ch <= 'Z'){
			ch = ch + key;
			
			if(ch > 'Z'){
				ch = ch - 'Z' + 'A' - 1;
			}
			
			password[i] = ch;
		}
	}
}
void decrypt(char password[],int key)
{
    	char ch;
	int i;
	for(i = 0; password[i] != '\0'; ++i){
		ch = password[i];
		
		if(ch >= 'a' && ch <= 'z'){
			ch = ch - key;
			
			if(ch < 'a'){
				ch = ch + 'z' - 'a' + 1;
			}
			
			password[i] = ch;
		}
		else if(ch >= 'A' && ch <= 'Z'){
			ch = ch - key;
			
			if(ch > 'a'){
				ch = ch + 'Z' - 'A' + 1;
			}
			
			password[i] = ch;
		}
	}
}
