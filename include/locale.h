#ifndef __LOCALE_H
#define __LOCALE_H

#undef NULL
#define NULL ((void *)0)

#define LC_CTYPE    0
#define LC_NUMERIC  1
#define LC_TIME	    2
#define LC_COLLATE  3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL	    6

#define LC_GLOBAL_LOCALE ((locale_t) - 1)

#define LC_CTYPE_MASK	 (1 << LC_CTYPE)
#define LC_NUMERIC_MASK	 (1 << LC_NUMERIC)
#define LC_TIME_MASK	 (1 << LC_TIME)
#define LC_COLLATE_MASK	 (1 << LC_COLLATE)
#define LC_MONETARY_MASK (1 << LC_MONETARY)
#define LC_MESSAGES_MASK (1 << LC_MESSAGES)
#define LC_ALL_MASK	 0x7fffffff

typedef struct __locale_t *locale_t;

struct lconv {
	char *currency_symbol;
	char *decimal_point;
	char frac_digits;
	char *grouping;
	char *int_curr_symbol;
	char int_frac_digits;
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_p_sign_posn;
	char *mon_decimal_point;
	char *mon_grouping;
	char *mon_thousands_sep;
	char *negative_sign;
	char n_cs_precedes;
	char n_sep_by_space;
	char n_sign_posn;
	char *positive_sign;
	char p_cs_precedes;
	char p_sep_by_space;
	char p_sign_posn;
	char *thousands_sep;
};

locale_t duplocale(locale_t);
void freelocale(locale_t);
const char *getlocalename_l(int, locale_t);
struct lconv *localeconv(void);
locale_t newlocale(int, const char *, locale_t);
char *setlocale(int, const char *);
locale_t uselocale(locale_t);

#endif
