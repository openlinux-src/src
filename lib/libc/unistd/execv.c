#include <unistd.h>
#include <syscall.h>

extern char **environ;

int execv(const char *path, char *const argv[])
{
	return syscall(execve, path, argv, environ);
}
