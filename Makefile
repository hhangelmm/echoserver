all: echoserver echoclient
echoserver:tcpserver_v02.c
	gcc -Wall -o echoserver tcpserver_v02.c -lpthread
echoclient:client_v04.c escommon.h escommon.c
	gcc -Wall -o echoclient client_v04.c escommon.c -lpthread
clean:
	rm echoserver echoclient
