#include <__statvfs.h>
#include <syscall.h>
#include <asm-generic/statfs.h>

int fstatvfs(int fildes, struct __statvfs *buf)
{
	struct statfs statfs;

	if (syscall(fstatfs, fildes, &statfs) < 0)
		return -1;

	buf->f_bsize = statfs.f_bsize;
	buf->f_frsize = statfs.f_frsize ? statfs.f_frsize : statfs.f_bsize;
	buf->f_blocks = statfs.f_blocks;
	buf->f_bfree = statfs.f_bfree;
	buf->f_bavail = statfs.f_bavail;
	buf->f_files = statfs.f_files;
	buf->f_ffree = statfs.f_ffree;
	buf->f_favail = statfs.f_ffree;
	buf->f_fsid = statfs.f_fsid.val[0];
	buf->f_flag = statfs.f_flags;
	buf->f_namemax = statfs.f_namelen;

	return 0;
}
