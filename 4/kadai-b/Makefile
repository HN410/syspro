tcpechoclient: tcpechoclient.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoclient.o mytcp.o -o tcpechoclient

tcpechoserver: tcpechoserver.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoserver.o mytcp.o -o tcpechoserver

tcpechoserver1: tcpechoserver1.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoserver1.o mytcp.o -o tcpechoserver1

tcpechoserver2: tcpechoserver2.o mytcp.o mytcp.h
	gcc -Wall -pthread tcpechoserver2.o mytcp.o -o tcpechoserver2


mytcp: mytcp.c
	gcc -c -Wall mytcp.c

tcpechoclient.o: tcpechoclient.c
	gcc -c -Wall -pthread tcpechoclient.c

tcpechoserver1.o: tcpechoserver1.c
	gcc -c -Wall -pthread tcpechoserver1.c

tcpechoserver2.o: tcpechoserver2.c
	gcc -c -Wall -pthread tcpechoserver2.c


clean: 
	rm mytcp.o tcpechoclient tcpechoclient.o tcpechoserver tcpechoserver.o