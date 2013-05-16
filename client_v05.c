#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <event2/event.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#define MAX_LINE 16384

void do_read(evutil_socket_t fd, short events, void *arg);
void do_write(evutil_socket_t fd, short events, void *arg);
void do_connect(evutil_socket_t sockfd, short event, void *arg);
struct event_base *base;
	struct event *sockfd_event;
int count=0;
int starttime,endtime,ttime;
struct sockaddr_in servaddr;
struct fd_state {
	char buffer[MAX_LINE];
	size_t buffer_used;

	size_t n_written;
	size_t write_upto;

	struct event *read_event;
	struct event *write_event;
};
typedef struct Canshu
{
	struct event_base *canshu_base;
	struct sockaddr_in canshu_servaddr;
}canshu;

void signalHandler(int sig){
	fprintf(stderr,"Catch signal Ctrl-C\n");
	endtime = time(NULL);
	ttime = difftime(endtime,starttime);
	//	count = 100000*bingfanum;
	float rps = (float)count/ttime;
	fprintf(stderr,"\ncount :%d ttime:%d rps:%f\n",count,ttime,rps);
	exit(0);
}

struct fd_state * alloc_fd_state(struct event_base *base, evutil_socket_t fd)
{
	struct fd_state *state = malloc(sizeof(struct fd_state));
	if (!state)
		return NULL;
	state->read_event = event_new(base, fd, EV_READ|EV_PERSIST, do_read, state);
	if (!state->read_event) {
		free(state);
		return NULL;
	}
	state->write_event =
		event_new(base, fd, EV_WRITE|EV_PERSIST, do_write, state);

	if (!state->write_event) {
		event_free(state->read_event);
		free(state);
		return NULL;
	}

	state->buffer_used = state->n_written = state->write_upto = 0;

	assert(state->write_event);
	return state;
}

void free_fd_state(struct fd_state *state)
{
	event_free(state->read_event);
	event_free(state->write_event);
	free(state);
}

void do_write(evutil_socket_t fd, short events, void *arg)
{
	struct fd_state *state = arg;
	char buf[] = "adsfa\n";
	ssize_t result;
	assert(state->read_event);
	result = send(fd, buf, sizeof(buf)-1, 0);
	if (result <= 0)
		perror("send:");
	assert(state->read_event);
	printf("write buf size:%d : %s\n",result,buf);
	count++;
	//event_add(state->read_event, NULL);
	event_del(state->write_event);
//	close(fd);
	return;	
}

void do_read(evutil_socket_t fd, short events, void *arg)
{
	struct fd_state *state = arg;
	char buf[1024];
	ssize_t result = recv(fd,buf,6,0);
	if (result < 0) {
		if (errno == EAGAIN) // XXX use evutil macro
			return;
		free_fd_state(state);
		return;
	}
	assert(result != 0);
	//	printf("read buf size:%d: %s\n",result,buf);
	//	close(fd);
	//	write(STDOUT_FILENO, buf, result);
	//	event_del(state->read_event);
	/*if(result==0)
	  close(fd);*/
	return;
}
int max(int a, int b)
{ 
	return a<b?b:a; 
} 


void do_connect(evutil_socket_t sockfd, short event, void *arg)
{
	canshu *cs = (canshu *)arg;
	struct event_base *base = cs->canshu_base;
	struct sockaddr_in servaddr = cs->canshu_servaddr;
	int fd = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (fd < 0 && (errno != EINPROGRESS)) { // XXXX eagain??
		perror("connect");
	} else {
		printf("connect %d:\n",sockfd);
		struct fd_state *state;
		evutil_make_socket_nonblocking(sockfd);
		state = alloc_fd_state(base, sockfd);
		assert(state); 
		assert(state->write_event);
		event_add(state->write_event, NULL);
	}
}

void run(int c, char **v)
{
	char o;
	int i,bingfanum=100;
	bzero(&servaddr,sizeof(servaddr));
	while(-1!=(o = getopt(c,v,
					"h:"
					"c:"
					))){
		switch(o){
			case 'c':
				bingfanum = atoi(optarg);
				break;
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
	servaddr.sin_port= htons(8000);

	evutil_socket_t sockfd[65000];


	base = event_base_new();
	if (!base)
		return; /*XXXerr*/

	canshu cs ;
	cs.canshu_base = base;
	cs.canshu_servaddr = servaddr;
	starttime = time(NULL);
	for(i=0;i<bingfanum;i++){
		sockfd[i] = socket(AF_INET,SOCK_STREAM,0);
		if(sockfd[i]==-1)
			perror("socket");
		evutil_make_socket_nonblocking(sockfd[i]);
		sockfd_event = event_new(base, sockfd[i], EV_READ|EV_PERSIST, do_connect,(void *)&cs);
		event_add(sockfd_event, NULL);
	}

	event_base_dispatch(base);
}

int main(int c, char **v)
{
	setvbuf(stdout, NULL, _IONBF, 0);
	signal(SIGINT,signalHandler);
	run(c,v);
	return 0;
}

