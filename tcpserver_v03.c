/*
 *echo server 服务端 v0.3 :select ,非阻塞I/O 
 */
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#define MAXLINE 80
#define SERV_PORT 8000
#define LISTENQ 20
void str_echo(int sockfd);
struct sockaddr_in cliaddr,servaddr;
int set_nonblocking(int fd);
fd_set rset,allset;
int main(int argc,char **argv)
{
	int i,maxfd,listenfd,connfd;
	int nready;
	socklen_t clilen;
	char str[INET_ADDRSTRLEN];
	if((listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket");
		exit(0);
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	int nREUSEADDR = 1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&nREUSEADDR,sizeof(int));
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		perror("bind");
		exit(0);
	}
	listen(listenfd,LISTENQ);
	//	printf("Accepting connections...\n");
	maxfd = FD_SETSIZE;

	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);
	for(;;){
		rset = allset;
		nready = select(maxfd,&rset,NULL,NULL,NULL);
		if(nready <0){
			perror("select");
			continue;
		}
		for(i=0;i<=maxfd;i++){
			if(FD_ISSET(i,&rset)){
				if(i != listenfd){
					str_echo(i);
				}else
				{
					clilen = sizeof(cliaddr);
					if((connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen))<0)
					{
						perror("accept");
						continue;
					}

					inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
					//			printf("connected from xxx  at  PORT %d,fd:%d\n",ntohs(cliaddr.sin_port),connfd);
					set_nonblocking(connfd);
					FD_SET(connfd,&allset);
				}
			}
		}
	}
}
void str_echo(int connfd)
{
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	ssize_t n;
	inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
		if((n = recv(connfd,buf,MAXLINE,0))>0){
			if(send(connfd,buf,n,0) <= 0)
				perror("write:");
		}else if (n==0){
			FD_CLR(connfd,&allset);
			close(connfd);
		}else if(n == EWOULDBLOCK){

		}else{
			perror("recv");
			FD_CLR(connfd,&allset);
			close(connfd);
		}
}
int set_nonblocking(int fd)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
