#include <stdarg.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <syscall.h>

int semctl(int semid, int semnum, int cmd, ...)
{
	va_list ap;
	struct semid_ds *buf = NULL;

	switch (cmd) {
	case SETVAL:
	case GETALL:
	case SETALL:
	case IPC_SET:
	case IPC_STAT:
		va_start(ap, cmd);
		buf = va_arg(ap, struct semid_ds *);
		va_end(ap);
	}

	return syscall(semctl, semid, semnum, cmd, buf);
}
