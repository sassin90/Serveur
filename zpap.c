#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

main() {
	unsigned int len;
	int  new;
	char msg[20];
	new=1;
	while(new){
		new=recv(0,msg,20,0);
		if(new){
			send(1,msg,new,0);
			
		} 
	}
	
}
