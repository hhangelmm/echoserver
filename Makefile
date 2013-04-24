all: echoserver echoclient
echoserver:tcpserver_v02.c
	gcc -o echoserver tcpserver_v02.c -lpthread
echoclient:client_v04.c escommon.h escommon.c
	gcc -o echoclient client_v04.c escommon.c -lpthread
clean:
	rm echoserver echoclient
