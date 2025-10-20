// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/ctype.h.html

#include "core.h"

#include <ctype.h>

TEST(ctype_h)
{
	TYPE(locale_t);

	FUNCTION(isalnum, int (*f)(int));
	FUNCTION(isalnum_l, int (*f)(int, locale_t));
	FUNCTION(isalpha, int (*f)(int));
	FUNCTION(isalpha_l, int (*f)(int, locale_t));
	FUNCTION(isblank, int (*f)(int));
	FUNCTION(isblank_l, int (*f)(int, locale_t));
	FUNCTION(iscntrl, int (*f)(int));
	FUNCTION(iscntrl_l, int (*f)(int, locale_t));
	FUNCTION(isdigit, int (*f)(int));
	FUNCTION(isdigit_l, int (*f)(int, locale_t));
	FUNCTION(isgraph, int (*f)(int));
	FUNCTION(isgraph_l, int (*f)(int, locale_t));
	FUNCTION(islower, int (*f)(int));
	FUNCTION(islower_l, int (*f)(int, locale_t));
	FUNCTION(isprint, int (*f)(int));
	FUNCTION(isprint_l, int (*f)(int, locale_t));
	FUNCTION(ispunct, int (*f)(int));
	FUNCTION(ispunct_l, int (*f)(int, locale_t));
	FUNCTION(isspace, int (*f)(int));
	FUNCTION(isspace_l, int (*f)(int, locale_t));
	FUNCTION(isupper, int (*f)(int));
	FUNCTION(isupper_l, int (*f)(int, locale_t));
	FUNCTION(isxdigit, int (*f)(int));
	FUNCTION(isxdigit_l, int (*f)(int, locale_t));
}
