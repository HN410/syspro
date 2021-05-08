iperfc: iperfc.o mytcp.o mytcp.h
	gcc -Wall iperfc.o mytcp.o -o iperfc

iperfs: iperfs.o mytcp.o mytcp.h
	gcc -Wall iperfs.o mytcp.o -o iperfs

iperfsG: iperfs.o mytcp.o mytcp.h
	gcc -g -Wall iperfs.o mytcp.o -o iperfs


mytcp: mytcp.c
	gcc -c -Wall mytcp.c

iperfc.o: iperfc.c
	gcc -c -Wall iperfc.c

iperfs.o: iperfs.c
	gcc -c -Wall iperfs.c

clean: 
	rm mytcp.o iperfc iperfc.o iperfs iperfs.o 