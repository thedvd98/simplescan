#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <netdb.h>

#include <err.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>

int vflag; //verbose
int uflag;	// udp default is tcp
char *pflag;
char *sflag;

int unix_connect(char *path)
{
	struct sockaddr_un s_un;
	int s, save_errno;

	if (uflag) {
		printf("Da implementare\n");
	} else {
		if ((s = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) < 0)
			return -1;
	}

	memset(&s_un, 0, sizeof(struct sockaddr_un));
	s_un.sun_family = AF_UNIX;

	if (strlcpy(s_un.sun_path, path, sizeof(s_un.sun_path)) >=
	    sizeof(s_un.sun_path)) {
		close(s);
		errno = ENAMETOOLONG;
		return -1;
	}
	if (connect(s, (struct sockaddr *)&s_un, sizeof(s_un)) < 0) {
		save_errno = errno;
		close(s);
		errno = save_errno;
		return -1;
	}
	return s;

}

/*
 * remote_connect()
 * Returns a socket connected to a remote host. Properly binds to a local
 * port or source address if needed. Returns -1 on failure.
 */
int remote_connect(const char *host, const char *port, struct addrinfo hints)
{
	struct addrinfo *res, *res0;
	int s = -1, error, on = 1, save_errno;

	if ((error = getaddrinfo(host, port, &hints, &res0)))
		errx(1, "getaddrinfo for host \"%s\" port %s: %s", host,
		    port, gai_strerror(error));

	for (res = res0; res; res = res->ai_next) {
		if ((s = socket(res->ai_family, res->ai_socktype |
		    SOCK_NONBLOCK, res->ai_protocol)) < 0)
			continue;

		/* Bind to a local port or source address if specified. */
		if (sflag || pflag) {
			struct addrinfo ahints, *ares;
#if defined  (SO_BINDANY)
			/* try SO_BINDANY, but don't insist */
			setsockopt(s, SOL_SOCKET, SO_BINDANY, &on, sizeof(on));
#endif
			memset(&ahints, 0, sizeof(struct addrinfo));
			ahints.ai_family = res->ai_family;
			ahints.ai_socktype = uflag ? SOCK_DGRAM : SOCK_STREAM;
			ahints.ai_protocol = uflag ? IPPROTO_UDP : IPPROTO_TCP;
			ahints.ai_flags = AI_PASSIVE;
			if ((error = getaddrinfo(sflag, pflag, &ahints, &ares)))
				errx(1, "getaddrinfo: %s", gai_strerror(error));

			if (bind(s, (struct sockaddr *)ares->ai_addr,
			    ares->ai_addrlen) < 0)
				err(1, "bind failed");
			freeaddrinfo(ares);
		}

		//set_common_sockopts(s, res->ai_family);

		if (connect(s, res->ai_addr, res->ai_addrlen) == 0) {
			printf("Connessione effettuta!\n");
			break;
		}
		if (vflag)
			warn("connect to %s port %s (%s) failed", host, port,
			    uflag ? "udp" : "tcp");

		save_errno = errno;
		close(s);
		errno = save_errno;
		s = -1;
	}

	freeaddrinfo(res0);

	return s;
}


int main(int argc, char *argv[])
{
	int s = -1;
	char *host;
	struct addrinfo hints;
	
	sflag = "127.0.0.1\0";
	pflag = "1234\0";
	host = NULL;
	vflag = 1;
	uflag = 0; // default TCP

	host = "127.0.0.1\0";
	printf("Connect to %s\n", host);

	remote_connect(host, pflag, hints);
	return 0;
}
