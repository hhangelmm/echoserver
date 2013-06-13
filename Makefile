all: echoserver echoclient
echoserver:tcpserver_v04.c 
	gcc -Wall -pg -g  -o echoserver tcpserver_v04.c  
echoclient:client_v04l.c    
	gcc -Wall -g -o echoclient client_v04l.c  -levent
clean:
	rm echoserver echoclient
