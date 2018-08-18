#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

int uflag;	// udp

int unix_connect(char *path)
{
	struct sockaddr_un s_un;
	int s, save_errno;

	if (uflag) {
		if ((s = unix_bind(unix_dg_tmp_socket, SOCK_CLOEXEC)) < 0)
			return -1;
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

int main() {

	uflag = 0; // default TCP

	if ((s = unix_connect(host)) > 0) {
		close(s);
	}

	return 0;
}
