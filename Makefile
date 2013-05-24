all: echoserver echoclient
echoserver:tcpserver_v03.c 
	gcc -Wall -pg -g  -o echoserver tcpserver_v03.c -lpthread 
echoclient:client_v03.c    
	gcc -Wall -g -o echoclient client_v03.c  -lpthread -levent
clean:
	rm echoserver echoclient
