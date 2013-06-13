all: echoserver echoclient
echoserver:tcpserver_v04.c 
	gcc -Wall -pg -g  -o echoserver tcpserver_v04.c  
echoclient:client_v03.c    
	gcc -Wall -g -o echoclient client_v03.c  -lpthread
clean:
	rm echoserver echoclient
