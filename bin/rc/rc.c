#include <unistd.h>
#include <string.h>
#include <stdio.h>

static void __init(void)
{
	pid_t pid;

	pid = fork();

	if (pid < 0) {
		perror("fork");
		_exit(1);
	}

	if (pid == 0) {
		execl("/bin/sh", "sh", NULL);
	}
}

int main(int argc, char **argv)
{
	if (argc > 1 && strcmp(argv[1], "__init") == 0) {
		__init();
	}

	return 0;
}
