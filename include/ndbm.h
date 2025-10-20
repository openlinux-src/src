#ifndef __NDBM_H
#define __NDBM_H

#define DBM_INSERT  0
#define DBM_REPLACE 1

typedef __SIZE_TYPE__ size_t;
typedef __UINT32_TYPE__ mode_t;

typedef struct __DBM DBM;

typedef struct {
	void *dptr;
	size_t dsize;
} datum;

int dbm_clearerr(DBM *);
void dbm_close(DBM *);
int dbm_delete(DBM *, datum);
int dbm_error(DBM *);
datum dbm_fetch(DBM *, datum);
datum dbm_firstkey(DBM *);
datum dbm_nextkey(DBM *);
DBM *dbm_open(const char *, int, mode_t);
int dbm_store(DBM *, datum, datum, int);

#endif
