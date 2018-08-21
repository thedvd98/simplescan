#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>	// atoi

#include <getopt.h>
#include <unistd.h> // per close()

int vflag; //verbose
int uflag;	// udp default is tcp

int s = -1; //socket descriptor

void usage() {
	printf("Usage: simplescan [-uv] host port\n");
}

int try_connect(struct sockaddr_in *sa)
{
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if (connect(s, (struct sockaddr *)sa, sizeof(*sa)) < 0) {
		close(s);
		return 0;
	}
	close(s);
	/*
	 * Porta aperta
	 */
	return 1;
}

int main(int argc, char *argv[])
{
	int ch;
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

	while ((ch = getopt(argc, argv, "uv")) != -1) {
		switch (ch) {
			case 'v':
				vflag = 1;
				break;
			case 'u':
				uflag = 1;
				break;
			default:
				usage();
				break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argv[0] && argv[1]) {
		//host = argv[0];
		//port = atoi(argv[1]);
		printf("host %s\n", host);
	} else {
		usage();
	}

	bzero(&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(host);

	sock_addr.sin_port = htons(port);

	if (try_connect(&sock_addr) == 1) {
		printf("Port %d is open!\n", port);
	} else {
		if (vflag) {
			printf("Port %d is closed\n", port);
		}
	}

	return 0;
}
