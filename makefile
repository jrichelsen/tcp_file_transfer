all:	tcpclient tcpserver

tcpclient:		tcpclient.o tcp_shared.o
					gcc -Wall tcpclient.o tcp_shared.o -o tcpclient
tcpserver:		tcpserver.o tcp_shared.o
					gcc -Wall tcpserver.o tcp_shared.o -o tcpserver

tcpclient.o:	tcpclient.c
					gcc -c -Wall tcpclient.c
tcpserver.o:	tcpserver.c
					gcc -c -Wall tcpserver.c
tcp_shared.o:	tcp_shared.h tcp_shared.c
					gcc -c -Wall tcp_shared.c

clean:
					rm -f *.o tcpclient tcpserver
