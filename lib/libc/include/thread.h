#ifndef __LIBC_THREAD_H
#define __LIBC_THREAD_H

struct __thread_self {
	int tid;
	int _errno;
};

#endif
