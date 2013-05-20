all: echoserver echoclient
echoserver:tcpserver_v01.c 
	gcc -Wall -pg -g  -o echoserver tcpserver_v01.c  
echoclient:client_v02.c    
	gcc -Wall -g -o echoclient client_v02.c  -lpthread -levent
clean:
	rm echoserver echoclient
