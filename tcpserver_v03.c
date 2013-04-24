/*
 * echo server  服务端 v0.3:阻塞I/O,并发模式：thread
 * */
#include "escommon.h"
static void *doit(void *);
void str_echo(int sockfd);
struct sockaddr_in servaddr,cliaddr;
int listenfd,connfd;
	char buf[MAXLINE];
int main(void)
{
	//	daemon(0,0);
	socklen_t cliaddr_len;
	int i;
	pthread_t tid;
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
				exit(0);
			}
		}
		pthread_create(&tid,NULL,&doit,(void*)connfd);
	}
}
static void* doit(void *arg)
{
	pthread_detach(pthread_self());
	str_echo((int)arg);
	close((int)arg);
	return(NULL);
}
void str_echo(int sockfd)
{
	int n,num;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
	while((n = read(connfd,buf,MAXLINE))>0){
		num++;
		printf("received from at PORT %d,%d\n",ntohs(cliaddr.sin_port),num);
		write(connfd,buf,n);
	}
	printf("the client has been closed.\n");
}
