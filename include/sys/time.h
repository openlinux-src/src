#ifndef __SYS_TIME_H
#define __SYS_TIME_H

#define __BITS_SELECT_H_
#include <bits/select.h>
#undef __BITS_SELECT_H_

int select(int, fd_set *restrict, fd_set *restrict, fd_set *restrict,
	   struct timeval *restrict);
int utimes(const char *, const struct timeval[2]);

#endif
