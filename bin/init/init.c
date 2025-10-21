#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mount.h>

#define major(x)                                      \
	((unsigned)((((x) >> 31 >> 1) & 0xfffff000) | \
		    (((x) >> 8) & 0x00000fff)))
#define minor(x) ((unsigned)((((x) >> 12) & 0xffffff00) | ((x) & 0x000000ff)))

#define makedev(x, y)                                                   \
	((((x) & 0xfffff000ULL) << 32) | (((x) & 0x00000fffULL) << 8) | \
	 (((y) & 0xffffff00ULL) << 12) | (((y) & 0x000000ffULL)))

int main(void)
{
	int sig;
	pid_t pid;
	sigset_t sigset;

	if (getpid() != 1) {
		return 1;
	}

	mknod("/dev/console", S_IFCHR | 0622, makedev(5, 1));
	mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));

	mount("proc", "/proc", "proc", 0, "");
	mount("sysfs", "/sys", "sysfs", 0, "");
	mount("tmpfs", "/dev", "tmpfs", 0, "");

	chdir("/");
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
