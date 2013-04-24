#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>
#define LISTENQ 20
#define MAXLINE 80
#define SERV_PORT 8000

void err_quit(char *str);
void err_sys(char *str);
int max(int a,int b);
