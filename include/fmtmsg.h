#ifndef __FMTMSG_H
#define __FMTMSG_H

#define MM_HARD	   0x001
#define MM_SOFT	   0x002
#define MM_FIRM	   0x004
#define MM_APPL	   0x008
#define MM_UTIL	   0x010
#define MM_OPSYS   0x020
#define MM_RECOVER 0x040
#define MM_NRECOV  0x080
#define MM_PRINT   0x100
#define MM_CONSOLE 0x200

#define MM_NOSEV   0x000
#define MM_HALT	   0x001
#define MM_ERROR   0x002
#define MM_WARNING 0x003
#define MM_INFO	   0x004

#define MM_NULLLBL ((char *)0)
#define MM_NULLSEV (0)
#define MM_NULLMC  ((long int)0L)
#define MM_NULLTXT ((char *)0)
#define MM_NULLACT ((char *)0)
#define MM_NULLTAG ((char *)0)

#define MM_NOTOK (-1)
#define MM_OK	 (0)
#define MM_NOMSG (1)
#define MM_NOCON (2)

int fmtmsg(long, const char *, int, const char *, const char *, const char *);

#endif
