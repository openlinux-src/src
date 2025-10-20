#ifndef __BITS_TIMESPEC_H
#define __BITS_TIMESPEC_H

#ifndef __BITS_TIMESPEC_H_
#error "Never include <bits/timespec.h> directly; use <time.h> instead."
#endif

typedef __INT64_TYPE__ time_t;

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

#endif
