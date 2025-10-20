#ifndef __LIBC_SELECT_H
#define __LIBC_SELECT_H

struct __fd_set {
	unsigned long fds_bits[16];
};

#endif
