#ifndef __FTW_H
#define __FTW_H

#define __BITS_STAT_H_
#include <bits/stat.h>
#undef __BITS_STAT_H_

#define FTW_F	0
#define FTW_D	1
#define FTW_DNR 2
#define FTW_DP	3
#define FTW_NS	4
#define FTW_SL	5
#define FTW_SLN 6

#define FTW_PHYS  0x01
#define FTW_MOUNT 0x02
#define FTW_XDEV  0x04
#define FTW_DEPTH 0x08
#define FTW_CHDIR 0x10

struct FTW {
	int base;
	int level;
};

int nftw(const char *,
	 int (*)(const char *, const struct stat *, int, struct FTW *), int,
	 int);

#endif
