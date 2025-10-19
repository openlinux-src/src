#ifndef __FNMATCH_H__
#define __FNMATCH_H__

#define FNM_PATHNAME   0x1
#define FNM_NOESCAPE   0x2
#define FNM_PERIOD     0x4
#define FNM_CASEFOLD   0x8
#define FNM_NOMATCH    1
#define FNM_IGNORECASE FNM_CASEFOLD

int fnmatch(const char *, const char *, int);

#endif
