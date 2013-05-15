/*
 * echo server  服务端 v0.0
 * */
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 80
#define SERV_PORT 8000
#define LISTENQ 20

int main(void)
{
	struct sockaddr_in servaddr,cliaddr;
	socklen_t cliaddr_len;
	int listenfd,connfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int n,num=0;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	listen(listenfd,20);
	printf("Accepting connections...\n");
	while(1){
		cliaddr_len = sizeof(cliaddr);
		if((connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&cliaddr_len))<0){
			if(errno == EINTR)
				continue;
			else{
				perror("accept:");
			exit(0);
			}
		}
		inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
		while((n = read(connfd,buf,MAXLINE))>0){
			num++;
			printf("received from at PORT %d,%d\n",ntohs(cliaddr.sin_port),num);
			write(connfd,buf,n);
		}
		printf("the client has been closed.\n");
		close(connfd);
	}
}
