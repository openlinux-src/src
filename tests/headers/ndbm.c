// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/netdb.h.html

#include "core.h"

#include <ndbm.h>

TEST(ndbm_h)
{
	TYPE(datum);
	STRUCT_MEMBER(datum, void *, dptr);
	STRUCT_MEMBER(datum, size_t, dsize);

	TYPE(size_t);

	INCOMPLETE_TYPE(DBM);
	MACRO(DBM_INSERT);
	MACRO(DBM_REPLACE);

	FUNCTION(dbm_clearerr, FN(int, DBM *));
	FUNCTION(dbm_close, FN(void, DBM *));
	FUNCTION(dbm_delete, FN(int, DBM *, datum));
	FUNCTION(dbm_error, FN(int, DBM *));
	FUNCTION(dbm_fetch, FN(datum, DBM *, datum));
	FUNCTION(dbm_firstkey, FN(datum, DBM *));
	FUNCTION(dbm_nextkey, FN(datum, DBM *));
	FUNCTION(dbm_open, FN(DBM *, const char *, int, mode_t));
	FUNCTION(dbm_store, FN(int, DBM *, datum, datum, int));

	TYPE(mode_t);
}
