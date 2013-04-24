/*
 * echo server 客户端 v0.1 :阻塞I/O
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
	int n;
	char sendline[MAXLINE],recvline[MAXLINE];
	while((n=read(STDIN_FILENO,sendline,MAXLINE))>0){
		write(sockfd,sendline,n);
		if((n=read(sockfd,recvline,MAXLINE))==0)
			err_quit("str_cli:server terminated prematurely");
		write(STDOUT_FILENO,recvline,n);
	}
}
