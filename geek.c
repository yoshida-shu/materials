#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int
main(int argc, char *argv[])
{
 struct sockaddr_in server;
 int sock;
 char buf[32];
 char *deststr;
 int n;

 if (argc != 2) {
	 printf("Usage : %s dest\n", argv[0]);
	 return 1;
 }
 deststr = argv[1];

 sock = socket(AF_INET, SOCK_STREAM, 0);

 server.sin_family = AF_INET;
 server.sin_port = htons(80);

 server.sin_addr.s_addr = inet_addr(deststr);
 if (server.sin_addr.s_addr == 0xffffffff) {
	 struct hostent *host;

	 host = gethostbyname(deststr);
	 if (host == NULL) {
		 return 1;
	 }
	 server.sin_addr.s_addr =
             *(unsigned int *)host->h_addr_list[0];
 }

 if(connect(sock, (struct sockaddr *)&server, sizeof(server))==-1)perror("connect");

 memset(buf, 0, sizeof(buf));
 n = read(sock, buf, sizeof(buf));

 printf("%d, %s\n", n, buf);

 close(sock);

 return 0;
}
