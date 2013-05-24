/*
 * echo server 客户端 v0.4:阻塞I/O,使用thread并发链接,可以用来测试 tcpserver_v03.c
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#define MAXLINE 80
#define SERV_PORT 8000

void *connectto(void* arg);
void myprintf(int i,char *str);
void str_cli(FILE *fp,int sockfd,int requestnum);

long int count;
struct sockaddr_in servaddr;

int requestnum = 2;
int printinfo = 0;
int keepalive = 0;
int main(int argc,char *argv[])
{
	char c;
	int count= 0;
	int currentnum = 1;
	int starttime,endtime,ttime;
	float rps;
	bzero(&servaddr,sizeof(servaddr));
	while(-1!=(c = getopt(argc,argv,
					"h:"
					"c:"
					"k"//是否采用长连接
					"n:" //发起请求的数量
					"v" //打印详细信息
					))){
		switch(c){
			case 'n': 
				requestnum = atoi(optarg);
				break;
			case 'c':
				currentnum = atoi(optarg);
				break;
			case 'h':
				inet_pton(AF_INET,optarg,&servaddr.sin_addr);
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
	servaddr.sin_port= htons(SERV_PORT);
	int i,rtn ;
	pthread_t tid1[currentnum];
	starttime = time(NULL);
	for(i = 0;i<currentnum;i++){
		if((rtn = pthread_create(&tid1[i],NULL,connectto,NULL))){
			printf("strerror: %s ", strerror(errno)); 
			printf("pthread error %d\n",i);
			exit(0);
		}
	}
	for(i = 0;i<currentnum;i++){
		pthread_join(tid1[i],NULL);
	}
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
	count = requestnum*currentnum;
	rps = (float)count/ttime;
	printf("\ncount :%d ttime:%d rps:%f\n",count,ttime,rps);
	return 0;
}
void *connectto(void* arg)
{
	int sockfd;
	char buf[MAXLINE];
	if(keepalive){
		myprintf(printinfo,"Long connection:");
		sockfd = socket(AF_INET,SOCK_STREAM,0);
		if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
		{
			perror("connect error");
		}else
		{
			snprintf(buf,sizeof(buf),"connect success %d",sockfd);
			myprintf(printinfo,buf);
			str_cli(stdin,sockfd,requestnum);
		}
		close(sockfd);
	}else{
		int i;
		myprintf(printinfo,"short connection:");
		for(i=0;i<requestnum;i++){
			sockfd = socket(AF_INET,SOCK_STREAM,0);
			if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
			{
				perror("connect error");
			}else
			{
				snprintf(buf,sizeof(buf),"connect success %d",sockfd);
				myprintf(printinfo,buf);
				str_cli(stdin,sockfd,1);
			}
			close(sockfd);
		}
	}
	return(NULL);
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
