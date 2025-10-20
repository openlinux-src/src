#ifndef __MQUEUE_H
#define __MQUEUE_H

#define __BITS_TIMESPEC_H_
#include <bits/timespec.h>
#undef __BITS_TIMESPEC_H_

#undef O_RDONLY
#define O_RDONLY 000000000
#undef O_WRONLY
#define O_WRONLY 000000001
#undef O_RDWR
#define O_RDWR 00000002

#undef O_CREAT
#define O_CREAT 00000100
#undef O_EXCL
#define O_EXCL 00000200
#undef O_NONBLOCK
#define O_NONBLOCK 00004000

typedef int mqd_t;
typedef __SIZE_TYPE__ size_t;
typedef __INT64_TYPE__ ssize_t;

struct sigevent;
struct mq_attr {
	long mq_flags;
	long mq_maxmsg;
	long mq_msgsize;
	long mq_curmsgs;
};

int mq_close(mqd_t);
int mq_getattr(mqd_t, struct mq_attr *);
int mq_notify(mqd_t, const struct sigevent *);
mqd_t mq_open(const char *, int, ...);
ssize_t mq_receive(mqd_t, char *, size_t, unsigned *);
int mq_send(mqd_t, const char *, size_t, unsigned);
int mq_setattr(mqd_t, const struct mq_attr *restrict, struct mq_attr *restrict);
ssize_t mq_timedreceive(mqd_t, char *restrict, size_t, unsigned *restrict,
			const struct timespec *restrict);
int mq_timedsend(mqd_t, const char *, size_t, unsigned,
		 const struct timespec *);
int mq_unlink(const char *);

#endif
