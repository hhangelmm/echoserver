/*
 * echo server 客户端 v0.4:阻塞I/O,使用thread并发链接
 */
#include "escommon.h"

void *connectto(void* arg);
void str_cli(FILE *fp,int sockfd);

long int count;
struct sockaddr_in servaddr;

int main(int argc,char *argv[])
{
	char c;
	char buf[] = "adsf";
	int sockfd,n,count= 0;
	int starttime,endtime,ttime;
	float rps;
	int bingfanum=1000;
	while(-1!=(c = getopt(argc,argv,
		"h:"
		"c:"
		))){
		switch(c){
			case 'c':
				bingfanum = (int)optarg;
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
	servaddr.sin_port= htons(SERV_PORT);
	starttime = time(NULL);
	int i ;
	pthread_t tid1[bingfanum];
	for(i = 0;i<bingfanum;i++){
		pthread_create(&tid1[i],NULL,connectto,(void*)sockfd);
		printf("\nthread create :%d \n",i);
	}
	for(i = 0;i<bingfanum;i++){
		pthread_join(tid1[i],NULL);
	}
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
	rps = (float)count/ttime;
	printf("\ncount :%d ttime:%d rps:%f\n",count,ttime,rps);
	return 0;
}
void *connectto(void* arg)
{
	int sockfd = (int)arg;
	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	str_cli(stdin,sockfd);
	close(sockfd);
}
void str_cli(FILE *fp,int sockfd)
{
	long int count = 0;
	int n;
	char recvline[MAXLINE];
	int m=10000;
	char *buf = "asdf";
	while((m--)>0){
	    write(sockfd,buf,strlen(buf));
		if((n = (read(sockfd,recvline,MAXLINE)))>0){
			count++;
			//write(STDOUT_FILENO,recvline,n);
		}
	}
	printf("\ncount :%ld \n",count);
}

