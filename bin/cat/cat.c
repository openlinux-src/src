#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void usage()
{
	fprintf(stderr, "usage: cat [-u] [file...]\n");
}

int main(int argc, char **argv)
{
	char opt;
	int fd;

	while ((opt = getopt(argc, argv, "u")) != -1) {
		switch (opt) {
		case 'u':
			/* unused */
			break;
		default:
			usage();
			return 1;
		}
	}

	for (int i = optind; i < argc; i++) {
		if (strcmp(argv[i], "-") == 0) {
			fd = STDIN_FILENO;
		} else {
			fd = open(argv[i], O_RDONLY);
			if (fd < 0) {
				printf("cat: %s: %s\n", argv[i],
				       strerror(errno));
				continue;
			}
		}

		char buf[BUFSIZ];
		ssize_t n;
		while ((n = read(fd, buf, sizeof(buf))) > 0) {
			ssize_t written = 0;
			while (written < n) {
				ssize_t w = write(STDOUT_FILENO, buf + written,
						  n - written);
				if (w < 0) {
					perror("write");
					break;
				}
				written += w;
			}
		}

		if (n < 0) {
			perror("read");
		}

		if (fd != STDIN_FILENO) {
			close(fd);
		}
	}

	return 0;
}
