#ifndef __POLL_H
#define __POLL_H

#define POLLIN	   0x0001
#define POLLRDNORM 0x0040
#define POLLRDBAND 0x0080
#define POLLPRI	   0x0002
#define POLLOUT	   0x0004
#define POLLWRNORM 0x0100
#define POLLWRBAND 0x0200
#define POLLERR	   0x0008
#define POLLHUP	   0x0010
#define POLLNVAL   0x0020

typedef __INT64_TYPE__ time_t;
typedef __INT32_TYPE__ sigset_t;
typedef unsigned long nfds_t;

struct timespec;

struct pollfd {
	int fd;
	short events;
	short revents;
};

int poll(struct pollfd[], nfds_t, int);
int ppoll(struct pollfd[], nfds_t, const struct timespec *restrict,
	  const sigset_t *restrict);

#endif
