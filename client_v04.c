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
	int sockfd,count= 0;
	int starttime,endtime,ttime;
	float rps;
	int bingfanum=10;
	bzero(&servaddr,sizeof(servaddr));
	while(-1!=(c = getopt(argc,argv,
					"h:"
					"c:"
					))){
		switch(c){
			case 'c':
				bingfanum = atoi(optarg);
			case 'h':
				inet_pton(AF_INET,optarg,&servaddr.sin_addr);
				break;
			default:
				printf("usage: echoclient -h<serverhost>");
				exit(0);
				break;
		}
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port= htons(SERV_PORT);
	int i,rtn ;
	pthread_t tid1[bingfanum];
	starttime = time(NULL);
	for(i = 0;i<bingfanum;i++){
		sockfd = socket(AF_INET,SOCK_STREAM,0);
		if((rtn = pthread_create(&tid1[i],NULL,connectto,(void*)sockfd))){
			printf("strerror: %s ", strerror(errno)); 
			printf("pthread error %d\n",i);
			exit(0);
		}
	}
	for(i = 0;i<bingfanum;i++){
		pthread_join(tid1[i],NULL);
	}
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
	count = 100000*bingfanum;
	rps = (float)count/ttime;
	printf("\ncount :%d ttime:%d rps:%f\n",count,ttime,rps);
	return 0;
}
void *connectto(void* arg)
{
	int sockfd = (int)arg;
	printf("\n%d",sockfd);

	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
	{
		printf("connect error: %s ", strerror(errno));
		exit(0);
	}
	str_cli(stdin,sockfd);
	close(sockfd);
	return(NULL);
}
void str_cli(FILE *fp,int sockfd)
{
	long int count = 0;
	int n;
	char recvline[MAXLINE];
	int m=100000;
	char *buf = "asdf\n";
	while((m--)>0){
		if( (n = write(sockfd,buf,strlen(buf))) < 0){
			perror("write error\n");
		}
		if((n = (read(sockfd,recvline,n)))>0){
			count++;
			//write(STDOUT_FILENO,recvline,n);
		}else
			perror("read error\n");
	}
	printf("\ncount :%ld %d ",count,sockfd);
	return;
}

