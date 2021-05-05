tcpechoclient: tcpechoclient.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoclient.o mytcp.o -o tcpechoclient

tcpechoserver: tcpechoserver.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoserver.o mytcp.o -o tcpechoserver

mytcp: mytcp.c
	gcc -c -Wall mytcp.c

tcpechoclient.o: tcpechoclient.c
	gcc -c -Wall -pthread tcpechoclient.c

tcpechoserver.o: tcpechoserver.c
	gcc -c -Wall -pthread tcpechoserver.c

clean: 
	rm mytcp.o tcpechoclient tcpechoclient.o tcpechoserver tcpechoserver.o