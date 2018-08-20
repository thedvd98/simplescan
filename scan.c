#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>

int vflag; //verbose
int uflag;	// udp default is tcp

int main(int argc, char *argv[])
{
	int s = -1;
	char *host;
	struct sockaddr_in sock_addr;

	host = "127.0.0.1\0";
	int port = 1234;
	/*
	 * Verbose flag, default enabled
	 */
	vflag = 1;
	/*
	 * UDP flag, default disabled
	 */
	uflag = 0;

	host = "127.0.0.1\0";
	printf("Connect to %s:%d\n", host, port);

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Errore creazione socket\n");
		return 1;
	}

	bzero(&sock_addr, sizeof(sock_addr));

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = inet_addr(host);
	if (connect(s, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
		perror("connect");
		close(s);
		return 2;
	}

	printf("Connessione effettuta\n");

	close(s);
	return 0;
}
