/*
 * echo server  服务端 v0.2,多线程处理并发
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

#include <pthread.h>
#define MAXLINE 80
#define SERV_PORT 8000
#define LISTENQ 20
static void* pstr_echo(void *arg);
void str_echo(int sockfd);
struct sockaddr_in servaddr,cliaddr;
int n,num=0;
int main(void)
{
	//  daemon(0,0);
	socklen_t cliaddr_len;
	int listenfd,connfd;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	int nREUSEADDR = 1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&nREUSEADDR,sizeof(int));
	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	listen(listenfd,20);
	printf("Accepting connections...\n");
	pthread_t tid;
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
		char is_long ;
		recv(connfd,&is_long,1,0);
		//		printf("connfd is_long:%c\n",is_long);
		if(is_long == 'k')
		{
			if(pthread_create(&tid,NULL,&pstr_echo,(void*)connfd))
			{
				perror("pthread_create error");
				close(connfd);
				exit(1);
			}
		}else
		{
			str_echo(connfd);
		}
	}
}	
static void* pstr_echo(void *arg)
{
	char buf[MAXLINE];
	int connfd = (int)arg;
	char str[INET_ADDRSTRLEN];
	struct sockaddr_in cliaddr;
	ssize_t n,num=0;
	inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
	while((n = recv(connfd,buf,MAXLINE,0))>0){
		num++;
		//      printf("received from at PORT %d,%d\n",ntohs(cliaddr.sin_port),num);
		send(connfd,buf,n,0);
	}
	//      printf("the client has been closed.\n");
	close(connfd);
	return(NULL);
}
void str_echo(int connfd)
{
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
	while((n = recv(connfd,buf,MAXLINE,0))>0){
		num++;
		//      printf("received from at PORT %d,%d\n",ntohs(cliaddr.sin_port),num);
		send(connfd,buf,n,0);
	}
	//      printf("the client has been closed.\n");
	close(connfd);
}
