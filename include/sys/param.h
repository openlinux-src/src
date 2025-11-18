#ifndef __SYS_PARAM_H
#define __SYS_PARAM_H

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define offsetof(s, e) __builtin_offsetof(s, e)

#define nitems(_a) (sizeof((_a)) / sizeof((_a)[0]))

#endif
