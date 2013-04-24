/*
 *echo server 服务端 v0.2 :阻塞I/O,读取时使用select ,并发:thread 
 */
#include "escommon.h"
static void* doit(void *);
int main(int argc,char **argv)
{
	int i,maxi,maxfd,listenfd,connfd,sockfd;
	int nready,client[FD_SETSIZE];
	fd_set rset,allset;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	char str[INET_ADDRSTRLEN];
	pthread_t tid;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	listen(listenfd,LISTENQ);
	printf("Accepting connections...\n");
	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i<FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);
	for(;;){
		rset = allset;
		nready = select(maxfd+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(listenfd,&rset)){
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
			inet_ntop(AF_INET, &cliaddr.sin_addr, str,sizeof(str));
			printf("connected from xxx  at  PORT %d\n",ntohs(cliaddr.sin_port));
			for(i=0;i<FD_SETSIZE;i++){
				if(client[i]<0){
					client[i] =connfd;
					break;
				}
			}
			if(i==FD_SETSIZE){
				printf("too many client");
				exit(0);
			}
			FD_SET(connfd,&allset);
			if(connfd>maxfd)
				maxfd = connfd;
			if(i>maxi)
				maxi = i;
			if(--nready <=0)
				continue;

		}
		for(i=0;i<=maxi;i++){
			if((sockfd = client[i])<0)
				continue;
			if(FD_ISSET(sockfd,&rset)){
				pthread_create(&tid,NULL,&doit,(void*)sockfd);
				FD_CLR(sockfd,&allset);
				client[i]=-1;
				printf("the client has been closed.\n");
				if(--nready <=0)
					break;
			}
		}
	}
}
static void* doit(void *arg)
{

	char line[MAXLINE];
	int sockfd = (int)arg;
	ssize_t n;
	pthread_detach(pthread_self());
	while((n = read(sockfd,line,MAXLINE))>0){
		write(sockfd,line,n);
	}
				close(sockfd);
	return(NULL);
}
