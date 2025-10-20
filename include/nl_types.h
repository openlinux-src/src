#ifndef __NL_TYPES_H
#define __NL_TYPES_H

#define NL_SETD	      1
#define NL_CAT_LOCALE 1

typedef int nl_item;
typedef void *nl_catd;

int catclose(nl_catd);
char *catgets(nl_catd, int, int, const char *);
nl_catd catopen(const char *, int);

#endif
