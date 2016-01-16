#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

main() {
	struct sockaddr_in remote;
	int sock;
	unsigned int len;
	int  new=1;
	char input[20];
	char output[20];
	struct sockaddr_in server,client;
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1) exit(-1);
	remote.sin_family= AF_INET;
	remote.sin_port = htons(4446);
	remote.sin_addr.s_addr= inet_addr("127.0.0.1");
	bzero(&remote.sin_zero,8);
	len=sizeof(struct sockaddr_in);
	if(connect(sock,(struct sockaddr *)&remote,len)==-1) exit(-1);
	while(1){
		fgets(input,20,stdin);
		send(sock,input,strlen(input),0);
		new=recv(sock,output,20,0);
		output[new]='\0';
		printf("message : %s\n",output);
	}
	close(sock);

}
