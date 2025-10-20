#include <libc.h>
#include <devctl.h>
#include <errno.h>
#include <syscall.h>

int posix_devctl(int fildes, int dcmd, void *restrict dev_data_ptr,
		 size_t unused nbyte, int *restrict dev_info_ptr)
{
	long r;

	if ((r = syscall(ioctl, fildes, dcmd, dev_data_ptr)) < 0)
		return errno;

	*dev_info_ptr = r;

	return 0;
}
