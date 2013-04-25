/*
 * echo server 客户端 v0.3:非阻塞I/O,使用select 
 */
#include "escommon.h"

void str_cli(FILE *fp,int sockfd);
char * gf_time();
int main(int argc,char *argv[])
{
	struct sockaddr_in servaddr;
	char buf[] = "adsf";
	int sockfd,n,count= 0;
	int starttime,endtime,ttime;
	float rps;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET,"10.227.13.232",&servaddr.sin_addr);
	servaddr.sin_port= htons(SERV_PORT);
	starttime = time(NULL);
	connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	str_cli(stdin,sockfd);
	close(sockfd);
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
//	rps = count/ttime;
//	printf("rps:%f",rps);
	return 0;
}
void str_cli(FILE *fp,int sockfd)
{
	int count,maxfdp1,val,stdineof;
	ssize_t n,nwritten,m,mwritten;
	fd_set rset,wset;
	char to[MAXLINE],fr[MAXLINE];
	char *toiptr, *tooptr,*friptr,*froptr;
	val = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,val|O_NONBLOCK);
	val = fcntl(STDIN_FILENO,F_GETFL,0);
	fcntl(STDIN_FILENO,F_SETFL,val|O_NONBLOCK);
	val = fcntl(STDOUT_FILENO,F_GETFL,0);
	fcntl(STDOUT_FILENO,F_SETFL,val|O_NONBLOCK);
	toiptr = tooptr = to;
	friptr = froptr = fr;
	stdineof = 0;
	maxfdp1 = max(max(STDIN_FILENO,STDOUT_FILENO),sockfd)+1;
	for(;;){
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		if(stdineof == 0 && toiptr<&to[MAXLINE])
			FD_SET(STDIN_FILENO,&rset); /* read from stdin*/
		if(friptr < &fr[MAXLINE])
			FD_SET(sockfd,&rset);/*read from socket*/
		if(tooptr!=toiptr)
			FD_SET(sockfd,&wset);/*data to write to socket*/
		if(froptr != friptr)
			FD_SET(STDOUT_FILENO,&wset);/*data to write to stdout*/
		select(maxfdp1,&rset,&wset,NULL,NULL);
		if(FD_ISSET(STDIN_FILENO,&rset)){
			if((n=read(STDIN_FILENO,toiptr,&to[MAXLINE]-toiptr))<0){
				if(errno != EWOULDBLOCK)
					err_sys("read error on stdin\n");
			}else if(n == 0){
				fprintf(stderr,"%s: EOF on stdin\n",gf_time());
				stdineof = 1;
				if(toiptr = tooptr)
					shutdown(sockfd,SHUT_WR);
			}else{
				fprintf(stderr,"%s : read %d butys from stdin\n",gf_time(),n);
				toiptr += n;
				FD_SET(sockfd,&wset);
			}
		}
		if(FD_ISSET(sockfd,&rset)){
			if((m=read(sockfd,friptr,&fr[MAXLINE]-friptr))<0){
				if(errno != EWOULDBLOCK)
					err_sys("read error on socket");
			}else if(m ==0){
				fprintf(stderr,"%s:EOF in socket\n",gf_time());
				if(stdineof)
					return;
				else
					err_quit("str_cli: server termlnated prematurely");
			}else {
				fprintf(stderr,"%s: read %d bytes from socket\n",gf_time(),m);
				friptr +=m;
				FD_SET(STDOUT_FILENO,&wset);
			}
		}
		if(FD_ISSET(STDOUT_FILENO,&wset) && ((m=friptr-froptr)>0)){
			if((nwritten = write(STDOUT_FILENO,froptr,m))<0){
				if(errno != EWOULDBLOCK)
					err_sys("write error to stdout\n");
				else
					err_sys("write stdout EWOULDBLOCK\n");
			}else{
				fprintf(stderr,"%s: wrote %d bytes to stdout\n",gf_time(),nwritten);
				froptr += nwritten;
				if(froptr == friptr)
					froptr = friptr = fr;
			}
		}
		if(FD_ISSET(sockfd,&wset) && ((n=toiptr-tooptr)>0)){
			if((mwritten = write(sockfd,tooptr,n))<0){
				if(errno != EWOULDBLOCK)
					err_sys("write error to socket\n");
			}else {
				fprintf(stderr,"%s:wrote %d bytes to socket\n",gf_time(),mwritten);
				tooptr += mwritten;
				if(tooptr == toiptr){
					toiptr = tooptr = to;
					if(stdineof)
						shutdown(sockfd,SHUT_WR);
				}
			}
		}
	}
}
char *gf_time(void)
{
	struct timeval tv;
	static char str[30];
	char *ptr;
	if(gettimeofday(&tv,NULL)<0)
		err_sys("gettimeofgay error");
	ptr = ctime(&tv.tv_sec);
	strcpy(str,&ptr[11]);
	snprintf(str+8,sizeof(str)-8,".%06ld",tv.tv_usec);
	return(str);

}
:
