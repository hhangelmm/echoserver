/*
 * echo server 客户端 v0.2 ，阻塞I/O，从描述字读取时使用select
 * */
#include "escommon.h"

void str_cli(FILE *fp,int sockfd);

int main(int argc,char **argv)
{
	int sockfd;
	char c;
	struct sockaddr_in servaddr;
	while(-1!=(c = getopt(argc,argv,
		"h:"))){
		switch(c){
			case 'h':
				inet_pton(AF_INET,optarg,&servaddr.sin_addr);
				break;
			default:
				printf("usage: echoclient -h<serverhost>");
				exit(0);
				break;
		}
	}
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	str_cli(stdin,sockfd);
	
	exit(0);
}
void str_cli(FILE *fp,int sockfd)
{
	int n,m;
	int maxfdp1,stdineof;
	fd_set rset;
	char sendline[MAXLINE],recvline[MAXLINE];
	stdineof=0;
	FD_ZERO(&rset);
	for(;;){
		if(stdineof==0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&rset)){
			if((n=read(sockfd,recvline,MAXLINE))==0){
				if(stdineof ==1)
					return;
				else
					err_quit("str_cli: server terminated prematurely");
			}
			write(STDOUT_FILENO,recvline,n);
		}
		if(FD_ISSET(fileno(fp),&rset)){
			if((m=read(STDIN_FILENO,sendline,MAXLINE))>0)
				write(sockfd,sendline,m);
			else if(m==0){
				stdineof = 1;
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}

		}
	}
}
