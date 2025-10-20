// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/locale.h.html

#include "core.h"

#include <locale.h>

TEST(locale_h)
{
	TYPE(struct lconv);
	STRUCT_MEMBER(struct lconv, char *, currency_symbol);
	STRUCT_MEMBER(struct lconv, char *, decimal_point);
	STRUCT_MEMBER(struct lconv, char, frac_digits);
	STRUCT_MEMBER(struct lconv, char *, grouping);
	STRUCT_MEMBER(struct lconv, char *, int_curr_symbol);
	STRUCT_MEMBER(struct lconv, char, int_frac_digits);
	STRUCT_MEMBER(struct lconv, char, int_n_cs_precedes);
	STRUCT_MEMBER(struct lconv, char, int_n_sep_by_space);
	STRUCT_MEMBER(struct lconv, char, int_n_sign_posn);
	STRUCT_MEMBER(struct lconv, char, int_p_cs_precedes);
	STRUCT_MEMBER(struct lconv, char, int_p_sep_by_space);
	STRUCT_MEMBER(struct lconv, char, int_p_sign_posn);
	STRUCT_MEMBER(struct lconv, char *, mon_decimal_point);
	STRUCT_MEMBER(struct lconv, char *, mon_grouping);
	STRUCT_MEMBER(struct lconv, char *, mon_thousands_sep);
	STRUCT_MEMBER(struct lconv, char *, negative_sign);
	STRUCT_MEMBER(struct lconv, char, n_cs_precedes);
	STRUCT_MEMBER(struct lconv, char, n_sep_by_space);
	STRUCT_MEMBER(struct lconv, char, n_sign_posn);
	STRUCT_MEMBER(struct lconv, char *, positive_sign);
	STRUCT_MEMBER(struct lconv, char, p_cs_precedes);
	STRUCT_MEMBER(struct lconv, char, p_sep_by_space);
	STRUCT_MEMBER(struct lconv, char, p_sign_posn);
	STRUCT_MEMBER(struct lconv, char *, thousands_sep);

	MACRO(NULL);

	MACRO(LC_ALL);
	MACRO(LC_COLLATE);
	MACRO(LC_CTYPE);
	MACRO(LC_MONETARY);
	MACRO(LC_NUMERIC);
	MACRO(LC_TIME);

	MACRO(LC_COLLATE_MASK);
	MACRO(LC_CTYPE_MASK);
	MACRO(LC_MESSAGES_MASK);
	MACRO(LC_MONETARY_MASK);
	MACRO(LC_NUMERIC_MASK);
	MACRO(LC_TIME_MASK);
	MACRO(LC_ALL_MASK);

	MACRO_TYPE(locale_t, LC_GLOBAL_LOCALE);
	TYPE(locale_t);

	FUNCTION(duplocale, FN(locale_t, locale_t));
	FUNCTION(freelocale, FN(void, locale_t));
	FUNCTION(localeconv, FN(struct lconv *));
	FUNCTION(newlocale, FN(locale_t, int, const char *, locale_t));
	FUNCTION(setlocale, FN(char *, int, const char *));
	FUNCTION(uselocale, FN(locale_t, locale_t));
}
