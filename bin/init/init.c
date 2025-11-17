#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mount.h>

int main(void)
{
	int sig;
	pid_t pid;
	sigset_t sigset;

	if (getpid() != 1) {
		return 1;
	}

	sigemptyset(&sigset);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	pid = fork();
	if (pid == 0) {
		char *const argv[] = { "/bin/rc", "__init", NULL };
		char *const envp[] = { NULL };

		sigprocmask(SIG_UNBLOCK, &sigset, NULL);
		setsid();

		execve("/bin/rc", argv, envp);

		perror("execl");
		_exit(1);
	}

	if (pid < 0) {
		perror("fork");
		return 1;
	}

	while (1) {
		alarm(30);
		sigwait(&sigset, &sig);
	}
}
