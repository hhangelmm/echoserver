all: echoserver echoclient
echoserver:tcpserver_v03.c
	gcc -Wall -o echoserver tcpserver_v03.c -lpthread -levent
echoclient:client_v05.c 
	gcc -Wall -o echoclient client_v05.c  -lpthread -levent
clean:
	rm echoserver echoclient
