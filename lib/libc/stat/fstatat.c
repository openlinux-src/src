#include <sys/stat.h>
#include <syscall.h>

#include <linux/stat.h>

#define makedev(major, minor)                \
	((((major) & 0xfffff000ULL) << 32) | \
	 (((major) & 0x00000fffULL) << 8) |  \
	 (((minor) & 0xffffff00ULL) << 12) | (((minor) & 0x000000ffULL)))

int fstatat(int fd, const char *restrict path, struct stat *restrict buf,
	    int flag)
{
	int ret;
	struct statx stx = { 0 };

	ret = syscall(statx, fd, path, flag, 0x7ff, &stx);

	if (ret == 0) {
		if (stx.stx_mask & STATX_BASIC_STATS) {
			buf->st_dev =
				makedev(stx.stx_dev_major, stx.stx_dev_minor);
			buf->st_ino = stx.stx_ino;
			buf->st_mode = stx.stx_mode;
			buf->st_nlink = stx.stx_nlink;
			buf->st_uid = stx.stx_uid;
			buf->st_gid = stx.stx_gid;
			buf->st_rdev =
				makedev(stx.stx_rdev_major, stx.stx_rdev_minor);
			buf->st_size = stx.stx_size;
			buf->st_atim.tv_sec = stx.stx_atime.tv_sec;
			buf->st_atim.tv_nsec = stx.stx_atime.tv_nsec;
			buf->st_mtim.tv_sec = stx.stx_mtime.tv_sec;
			buf->st_mtim.tv_nsec = stx.stx_mtime.tv_nsec;
			buf->st_ctim.tv_sec = stx.stx_ctime.tv_sec;
			buf->st_ctim.tv_nsec = stx.stx_ctime.tv_nsec;
			buf->st_blksize = stx.stx_blksize;
			buf->st_blocks = stx.stx_blocks;
		} else {
			ret = -1; // Indicate failure if STATX_BASIC_STATS is not set
		}
	}

	return ret;
}
