#ifndef __SYS_SELECT_H
#define __SYS_SELECT_H

#define __BITS_SELECT_H_
#include <bits/select.h>
#undef __BITS_SELECT_H_

#define __BITS_TIMESPEC_H_
#include <bits/timespec.h>
#undef __BITS_TIMESPEC_H_

typedef __INT32_TYPE__ sigset_t;

int pselect(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
	    const struct timespec *restrict, const sigset_t *restrict);
int select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
	   struct timeval *restrict);

#endif
