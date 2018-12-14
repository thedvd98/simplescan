#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>	// atoi

#include <getopt.h>
#include <unistd.h> // per close()

#define PORT_SEPARATOR '-'

int vflag; //verbose
int uflag;	// udp default is tcp

int s = -1; //socket descriptor

struct port_range {
	int startport;
	int endport;
};

void usage() {
	printf("Usage: simplescan [-uv] host port\n"
			"port puó essere una porta o un range di porte(Es. 20-40)\n");
}

int udptest(int s) {
	int ret;

	if (write(s, "A", 1) == 1)
		ret = 1;
	else
		ret = 0;
	return ret;
}
/*
 * Verifica lo stato della porta
 * TODO udp support
 * 1 se la porta é aperta
 * */
int try_connect(struct sockaddr_in *sa)
{
	if ((s = socket(AF_INET, (uflag ? SOCK_DGRAM : SOCK_STREAM), 0)) < 0) {
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

/*
 * TODO verificare la validitá di portstr
 */
void build_port_range(struct port_range *prange, char *portstr) {
	char *n;
	int sport, eport;

	if ((n = strchr(portstr, PORT_SEPARATOR)) == NULL) {
		/* Only one port */
		prange->startport = atoi(portstr);
		prange->endport = prange->startport;
	} else {
		n++; //salta il separatore
		sport = atoi(portstr);
		eport = atoi(n);

		if (sport > eport) {
			prange->startport = eport;
			prange->endport = sport;
		} else {
			prange->startport = sport;
			prange->endport = eport;
		}
	}
	printf("%d:%d\n", prange->startport, prange->endport);
}

int main(int argc, char *argv[])
{
	int ch;
	char *host, *port_str;
	struct sockaddr_in sock_addr;
	struct port_range p_range;
	/*
	 * Verbose flag, default enabled
	 */
	vflag = 0;
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
		host = argv[0];
		port_str = argv[1];
		printf("host %s\n", host);
	} else {
		usage();
		return 1;
	}

	bzero(&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(host);

	build_port_range(&p_range, port_str);

	for (int p = p_range.startport; p <= p_range.endport; p++) {
		sock_addr.sin_port = htons(p);

		if (try_connect(&sock_addr) == 1) {
			printf("Port %d is open!\n", p);
		} else {
			if (vflag) {
				printf("Port %d is closed\n", p);
			}
		}
	}

	return 0;
}
