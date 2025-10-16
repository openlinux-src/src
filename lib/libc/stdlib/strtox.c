#include <ctype.h>
#include <errno.h>
#include <strings.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

static unsigned long long
__intscan(const char *s, int base, unsigned long long lim, int *neg, char **end)
{
	unsigned long long res = 0;
	int digit, any = 0;

	while (isspace((unsigned char)*s))
		s++;

	*neg = 0;
	if (*s == '+' || *s == '-') {
		*neg = (*s == '-');
		s++;
	}

	if ((base == 0 || base == 16) && s[0] == '0' &&
	    (s[1] == 'x' || s[1] == 'X')) {
		base = 16;
		s += 2;
	} else if (base == 0 && *s == '0') {
		base = 8;
		s++;
	} else if (base == 0) {
		base = 10;
	}

	for (; *s; s++) {
		unsigned char c = *s;
		if (isdigit(c))
			digit = c - '0';
		else if (isalpha(c))
			digit = toupper(c) - 'A' + 10;
		else
			break;

		if (digit >= base)
			break;
		if (res > (lim - digit) / base) {
			errno = ERANGE;
			res = lim;
			any = 1;
			while (isalnum((unsigned char)*++s))
				; /* consume rest */
			break;
		}
		res = res * base + digit;
		any = 1;
	}

	if (end)
		*end = (char *)(any ? s : s - 1);

	return res;
}

static long double __scanfloat(const char *s, char **end)
{
	long double value = 0.0;
	long double frac = 0.0;
	long double sign = 1.0;
	long double scale = 1.0;
	int exp_sign = 1;
	long exp_val = 0;
	int got_digit = 0;

	while (isspace((unsigned char)*s))
		s++;

	if (*s == '+' || *s == '-') {
		if (*s == '-')
			sign = -1.0;
		s++;
	}

	if (strncasecmp(s, "inf", 3) == 0) {
		s += 3;
		if (strncasecmp(s, "inity", 5) == 0)
			s += 5;
		if (end)
			*end = (char *)s;
		return sign * INFINITY;
	}
	if (strncasecmp(s, "nan", 3) == 0) {
		s += 3;
		if (*s == '(') {
			s++;
			while (*s && *s != ')')
				s++;
			if (*s == ')')
				s++;
		}
		if (end)
			*end = (char *)s;
		return NAN;
	}

	while (isdigit((unsigned char)*s)) {
		value = value * 10.0 + (*s - '0');
		s++;
		got_digit = 1;
	}

	if (*s == '.') {
		s++;
		while (isdigit((unsigned char)*s)) {
			value = value * 10.0 + (*s - '0');
			scale *= 10.0;
			s++;
			got_digit = 1;
		}
	}

	if (!got_digit) {
		if (end)
			*end = (char *)s;
		return 0.0;
	}

	value = value / scale;

	if (*s == 'e' || *s == 'E') {
		const char *p = ++s;
		if (*p == '+' || *p == '-') {
			exp_sign = (*p == '-') ? -1 : 1;
			p++;
		}
		while (isdigit((unsigned char)*p)) {
			exp_val = exp_val * 10 + (*p - '0');
			p++;
		}
		s = p;
		value *= powl(10.0, exp_sign * exp_val);
	}

	if (end)
		*end = (char *)s;

	return sign * value;
}

long strtol(const char *restrict nptr, char **restrict endptr, int base)
{
	int neg;
	unsigned long long v = __intscan(nptr, base, LONG_MAX, &neg, endptr);
	if (neg)
		return (v > (unsigned long long)LONG_MAX + 1ULL) ?
			       (errno = ERANGE, LONG_MIN) :
			       -(long)v;
	else
		return (v > LONG_MAX) ? (errno = ERANGE, LONG_MAX) : (long)v;
}

long long strtoll(const char *restrict nptr, char **restrict endptr, int base)
{
	int neg;
	unsigned long long v = __intscan(nptr, base, LLONG_MAX, &neg, endptr);
	if (neg)
		return (v > (unsigned long long)LLONG_MAX + 1ULL) ?
			       (errno = ERANGE, LLONG_MIN) :
			       -(long long)v;
	else
		return (v > LLONG_MAX) ? (errno = ERANGE, LLONG_MAX) :
					 (long long)v;
}

unsigned long strtoul(const char *restrict nptr, char **restrict endptr,
		      int base)
{
	int neg;
	unsigned long long v = __intscan(nptr, base, ULONG_MAX, &neg, endptr);
	if (neg) {
		errno = EINVAL;
		return 0;
	}
	return (unsigned long)v;
}

unsigned long long strtoull(const char *restrict nptr, char **restrict endptr,
			    int base)
{
	int neg;
	unsigned long long v = __intscan(nptr, base, ULLONG_MAX, &neg, endptr);
	if (neg) {
		errno = EINVAL;
		return 0;
	}
	return v;
}

float strtof(const char *restrict nptr, char **restrict endptr)
{
	return (float)__scanfloat(nptr, endptr);
}

long double strtold(const char *restrict nptr, char **restrict endptr)
{
	return __scanfloat(nptr, endptr);
}
