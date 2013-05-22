/*
 * echo server 客户端 v0.1 :阻塞I/O,无并发
 * */
#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#define MAXLINE 80
#define SERV_PORT 8000

int printinfo= 0;
void str_cli(FILE *fp,int sockfd,int requestnum);
void myprintf(int i,char *str);
int main(int argc,char **argv)
{
	int sockfd;
	char c;
	struct sockaddr_in servaddr;
	int requestnum = 2;
	int keepalive = 0; // 短连接
	int starttime,endtime,ttime;
	int totalrequest = 0;
	float rps=0;
	char buf[MAXLINE];
	bzero(&servaddr,sizeof(servaddr));
	while(-1!=(c = getopt(argc,argv,
					"h:"
					"n:" //发起请求的数量
					"k" //采用长连接
					"v" //打印详细信息
					))){
		switch(c){
			case 'h':
				inet_pton(AF_INET,optarg,&servaddr.sin_addr);
				break;
			case 'n':
				requestnum = atoi(optarg);
				break;
			case 'k':
				keepalive = 1;
				break;
			case 'v':
				printinfo = 1;
				break;
			default:
				printf("usage: echoclient -h<serverhost> -n<requestnum> [-K]");
				exit(0);
				break;
		}
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	starttime = time(NULL); 
	if(keepalive){
		myprintf(printinfo,"Long connection:");
		sockfd = socket(AF_INET,SOCK_STREAM,0);
		if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==0)
		{
			snprintf(buf,sizeof(buf),"connect success %d",sockfd);
			myprintf(printinfo,buf);
			str_cli(stdin,sockfd,requestnum);
			totalrequest = requestnum;
		}else
			perror("connect:");
		close(sockfd);
	}else{
		myprintf(printinfo,"short connection:");
		totalrequest = requestnum;
		while(requestnum-->0){
			sockfd = socket(AF_INET,SOCK_STREAM,0);
			if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))==0)
			{
				snprintf(buf,sizeof(buf),"connect success %d",sockfd);
				myprintf(printinfo,buf);
				str_cli(stdin,sockfd,1);
			}else
				perror("connect:");
			close(sockfd);
		}
	}	
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
	rps = (float)totalrequest/ttime;
	printf("\ntotal request num :%d\n totaltime:%d\n rps:%f\n",totalrequest,ttime,rps);
	exit(0);
}
void str_cli(FILE *fp,int sockfd,int requestnum)
{
	int n;
	char buf[MAXLINE];
	char sendline[]="asdf\n",recvline[MAXLINE];
	while(requestnum-->0){
		n = strlen(sendline);
		if((n = send(sockfd,sendline,n,0))>0)
		{
			snprintf(buf,sizeof(buf),"write to server %d chars : %s",n,sendline);
			myprintf(printinfo,buf);
		}else
		{
			perror("write:");
		}
		if((n=recv(sockfd,recvline,MAXLINE,0))>0)
		{
			snprintf(buf,sizeof(buf),"read from server %d chars:  %s",n,recvline);
			myprintf(printinfo,buf);
			//write(STDOUT_FILENO,recvline,n);
		}
		else
			perror("read:");
	}
}
void myprintf(int i,char *str)
{
	if(i)
		printf("%s\n",str);
}
