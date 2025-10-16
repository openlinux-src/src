#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fildes;

	if (argc < 2) {
		write(STDOUT_FILENO, "nohup utility [argument...]\n", 29);
		return 1;
	}

	if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
		perror("nohup: signal");
		return 1;
	}

	if (isatty(STDOUT_FILENO)) {
		if ((fildes = open("nohup.out",
				   O_WRONLY | O_APPEND | O_CREAT)) < 0) {
			perror("nohup: open");
			return 1;
		}

		if (dup2(fildes, STDOUT_FILENO) < 0) {
			perror("nohup: dup2");
			return 1;
		}

		close(fildes);
	}

	if (isatty(STDERR_FILENO)) {
		if (dup2(STDOUT_FILENO, STDERR_FILENO) < 0) {
			perror("nohup: dup2");
			return 1;
		}
	}

	execvp(argv[0], argv);
	perror("nohup: execvp");
	exit(127 + (errno == ENOENT));
}
