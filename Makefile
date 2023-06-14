all: client server observer
Client: client.c header.h
	gcc client.c -o client
Server: server.c header.h
	gcc server.c -o server
Observer: observer.c header.h
	gcc observer.c -o observer
