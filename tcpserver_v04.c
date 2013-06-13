/*
 * echo server 服务端 v0.4 :epoll,非阻塞I/O
 */
#include  <unistd.h>
#include  <sys/types.h>    
#include  <sys/socket.h>   
#include  <netinet/in.h>   
#include  <arpa/inet.h>    
#include <sys/epoll.h>
#include <fcntl.h>    
#include <sys/resource.h> 
#include <linux/tcp.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAXEPOLLSIZE 200000
#define MAXLINE 80
#define SERV_PORT 8000 
#define LISTENQ 1024

int set_nonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int nread, listenfd, connfd, kdpfd, nfds, n,  curfds;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t socklen = sizeof(struct sockaddr_in);
	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];
	struct rlimit ;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (SERV_PORT);
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		return -1;
	}
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1) 
	{
		perror("bind error");
		return -1;
	} 
	if (listen(listenfd, LISTENQ) == -1) 
	{
		perror("listen error");
		return -1;
	}
	/* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
	kdpfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) 
	{
		fprintf(stderr, "epoll set insertion error: fd=%d\n", listenfd);
		return -1;
	}
	curfds = 1;
	const int on=1;
	setsockopt(listenfd,IPPROTO_TCP,TCP_NODELAY,&on,sizeof(on));
	printf("Accepting connections...\n");
	for (;;) {
		/* 等待有事件发生 */
		nfds = epoll_wait(kdpfd, events, curfds, -1);
		if (nfds == -1)
		{
			perror("epoll_wait");
			continue;
		}
		/* 处理所有事件 */
		for (n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listenfd) 
			{
				connfd = accept(listenfd, (struct sockaddr *)&cliaddr,&socklen);
				if (connfd < 0) 
				{
					perror("accept error");
					continue;
				}
			//	sprintf(buf, "accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
				//printf("accept form %s:%d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
				//printf("%d:%s", ++acceptCount, buf);
				if (curfds >= MAXEPOLLSIZE) {
					fprintf(stderr, "too many connection, more than %d\n", MAXEPOLLSIZE);
					close(connfd);
					continue;
				} 
				if (set_nonblocking(connfd) < 0) {
					perror("setnonblocking error");
				}
				ev.events = EPOLLIN;
				ev.data.fd = connfd;
				if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, connfd, &ev) < 0)
				{
					fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
					return -1;
				}
				curfds++;
//				printf("accept connect %d\n",connfd);
				continue;
			}else if(events[n].events&EPOLLIN) 
			{
			
				char buf[MAXLINE]={};
				connfd = events[n].data.fd;
				nread = recv(connfd, buf, MAXLINE,0);				
				if(nread > 0){
//					printf("recv connfd:%d bytes:%d,port:%d\n",connfd,nread,ntohs(cliaddr.sin_port));
					if(write(connfd, buf, strlen(buf)) <= 0){
					    perror("send:");
					}
//					printf("write connfd:%d bytes:%d,port:%d\n",connfd,nread,ntohs(cliaddr.sin_port));
				}else if(nread == 0){
					//epoll_ctl(kdpfd, EPOLL_CTL_DEL, connfd, &ev);
					close(connfd);
					curfds--;
					//printf("connection be closed %d\n",connfd);
					continue;
				}else{
					perror("read error");
				}
			}
		}
	}
	close(listenfd);
	return 0;
}
