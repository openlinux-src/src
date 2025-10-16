#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	double seconds;
	struct timespec ts;

	if (argc < 2) {
		write(STDOUT_FILENO, "usage: ", 7);
		write(STDOUT_FILENO, argv[0], strlen(argv[0]));
		write(STDOUT_FILENO, " seconds\n", 9);
		return 1;
	}

	seconds = atof(argv[1]);
	ts.tv_sec = (time_t)seconds;
	ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e9);
	nanosleep(&ts, NULL);

	return 0;
}
