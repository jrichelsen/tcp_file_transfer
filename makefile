all:	tcpclient tcpserver

tcpclient:		tcpclient.o tcp_shared.o
					gcc tcpclient.o tcp_shared.o -o tcpclient
tcpserver:		tcpserver.o tcp_shared.o
					gcc tcpserver.o tcp_shared.o -o tcpserver

tcpclient.o:	tcpclient.c
					gcc -c -g -O -Wall tcpclient.c
tcpserver.o:	tcpserver.c
					gcc -c -g -O -Wall tcpserver.c
tcp_shared.o:	tcp_shared.h tcp_shared.c
					gcc -c -g -O -Wall tcp_shared.c

clean:
					rm -f *.o tcpclient tcpserver
