#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <unistd.h>

extern char *dtoa(double, int mode, int ndigits, int *decpt, int *sign,
		  char **rve);

static int utoa_base(unsigned long long v, char *s, int b, int p, int u)
{
	const char *d = u ? "0123456789ABCDEF" : "0123456789abcdef";
	char buf[65];
	int i = 0, j;
	if (p < 0)
		p = 0;
	if (p > 64)
		p = 64;
	do {
		buf[i++] = d[v % b];
		v /= b;
	} while (v && i < 64);
	while (i < p && i < 64)
		buf[i++] = '0';
	for (j = 0; j < i; ++j)
		s[j] = buf[i - j - 1];
	s[i] = 0;
	return i;
}

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap)
{
	const char *ptr;
	int total_printed;

	ptr = format;
	total_printed = 0;

	while (*ptr != '\0') {
		if (*ptr != '%') {
			fputc(*ptr, stream);
			total_printed++;
			ptr++;
		} else {
			char *s = NULL;
			char buf[128];
			int l = 0;
			int width = 0;
			int padding = 0;
			int precision = -1;

			// Skip the '%'
			ptr++;

			enum {
				FLAG_NONE = 0,
				FLAG_MINUS = 1 << 0, // '-'
				FLAG_PLUS = 1 << 1, // '+'
				FLAG_HASH = 1 << 2, // '#'
				FLAG_ZERO = 1 << 3, // '0'
				FLAG_SPACE = 1 << 4 // ' '
			} flags = FLAG_NONE;

			enum {
				LENGTH_NONE, // default (no modifier)
				LENGTH_HH, // 'hh' -> signed char / unsigned char
				LENGTH_H, // 'h' -> short / unsigned short
				LENGTH_L, // 'l' -> long / unsigned long
				LENGTH_LL, // 'll' -> long long / unsigned long long
				LENGTH_J, // 'j' -> intmax_t / uintmax_t
				LENGTH_Z, // 'z' -> size_t
				LENGTH_T, // 't' -> ptrdiff_t
				LENGTH_CAPL // 'L' -> long double
			} length = LENGTH_NONE;

			while (*ptr == '-' || *ptr == '+' || *ptr == ' ' ||
			       *ptr == '0' || *ptr == '#') {
				if (*ptr == '-')
					flags |= FLAG_MINUS;
				else if (*ptr == '+')
					flags |= FLAG_PLUS;
				else if (*ptr == ' ')
					flags |= FLAG_SPACE;
				else if (*ptr == '0')
					flags |= FLAG_ZERO;
				else if (*ptr == '#')
					flags |= FLAG_HASH;
				ptr++;
			}

			if (*ptr == '*') {
				width = va_arg(ap, int);
				if (width < 0) {
					flags |= FLAG_MINUS;
					width = -width;
				}
				ptr++;
			} else {
				while (isdigit(*ptr)) {
					width = width * 10 + (*ptr - '0');
					ptr++;
				}
			}

			if (*ptr == '.') {
				ptr++;
				if (*ptr == '*') {
					precision = va_arg(ap, int);
					if (precision < 0)
						precision = -1;
					ptr++;
				} else {
					precision = 0;
					while (isdigit(*ptr)) {
						precision = precision * 10 +
							    (*ptr - '0');
						ptr++;
					}
				}
			}

			switch (*ptr) {
			case 'h':
				if (*(ptr + 1) == 'h') {
					length = LENGTH_HH;
					ptr++;
				} else
					length = LENGTH_H;
				ptr++;
				break;
			case 'l':
				if (*(ptr + 1) == 'l') {
					length = LENGTH_LL;
					ptr++;
				} else
					length = LENGTH_L;
				ptr++;
				break;
			case 'L':
				length = LENGTH_CAPL;
				ptr++;
				break;
			case 'j':
				length = LENGTH_J;
				ptr++;
				break;
			case 'z':
				length = LENGTH_Z;
				ptr++;
				break;
			case 't':
				length = LENGTH_T;
				ptr++;
				break;
			}

			switch (*ptr) {
			case 'i':
			case 'd': {
				long long val;
				int negative = 0;

				switch (length) {
				case LENGTH_NONE:
					val = va_arg(ap, int);
					break;
				case LENGTH_HH:
					val = (signed char)va_arg(ap, int);
					break;
				case LENGTH_H:
					val = (short)va_arg(ap, int);
					break;
				case LENGTH_L:
					val = va_arg(ap, long);
					break;
				case LENGTH_LL:
					val = va_arg(ap, long long);
					break;
				case LENGTH_J:
					val = va_arg(ap, intmax_t);
					break;
				case LENGTH_Z:
					val = va_arg(ap, size_t);
					break;
				case LENGTH_T:
					val = va_arg(ap, ptrdiff_t);
					break;
				case LENGTH_CAPL:
					errno = EINVAL;
					return -1;
				}

				l = utoa_base(val, buf, 10, precision, 0);

				s = buf;
				if (val < 0) {
					memmove(s + 1, s, l);
					s[0] = '-';
					s[++l] = '\0';
				} else if (flags & FLAG_PLUS) {
					memmove(s + 1, s, l);
					s[0] = '+';
					s[++l] = '\0';
				} else if (flags & FLAG_SPACE) {
					memmove(s + 1, s, l);
					s[0] = ' ';
					s[++l] = '\0';
				}

				break;
			}
			case 'o':
			case 'u':
			case 'x':
			case 'X': {
				unsigned long long val;

				switch (length) {
				case LENGTH_NONE:
					val = va_arg(ap, int);
					break;
				case LENGTH_HH:
					val = (signed char)va_arg(ap, int);
					break;
				case LENGTH_H:
					val = (short)va_arg(ap, int);
					break;
				case LENGTH_L:
					val = va_arg(ap, long);
					break;
				case LENGTH_LL:
					val = va_arg(ap, long long);
					break;
				case LENGTH_J:
					val = va_arg(ap, intmax_t);
					break;
				case LENGTH_Z:
					val = va_arg(ap, size_t);
					break;
				case LENGTH_T:
					val = va_arg(ap, ptrdiff_t);
					break;
				case LENGTH_CAPL:
					errno = EINVAL;
					return -1;
				}

				int base = (*ptr == 'o') ? 8 :
					   (*ptr == 'u') ? 10 :
							   16;
				int upper = (*ptr == 'X');
				l = utoa_base(val, buf, base, precision, upper);
				s = buf;

				if ((flags & FLAG_HASH) && val != 0) {
					if (*ptr == 'o') {
						memmove(s + 1, s, l);
						s[0] = '0';
						s[++l] = '\0';
					} else if (*ptr == 'x' || *ptr == 'X') {
						memmove(s + 2, s, l);
						s[0] = '0';
						s[1] = upper ? 'X' : 'x';
						s[(l += 2)] = '\0';
					}
				}
				break;
			}

			case 'f':
			case 'F':
			case 'e':
			case 'E':
			case 'g':
			case 'G': {
				long double val;

				if (length == LENGTH_CAPL)
					val = va_arg(ap, long double);
				else
					val = va_arg(ap, double);

				int mode, ndigits;

				if (precision == -1)
					precision = 6;

				switch (*ptr) {
				case 'f':
				case 'F':
					mode = 3;
					ndigits = precision;
					break;
				case 'e':
				case 'E':
					mode = 2;
					ndigits = precision + 1;
					break;
				case 'g':
				case 'G':
					mode = 0;
					ndigits = precision;
					break;
				}

				int decpt, sign;
				char *rve;
				char *buf_r = dtoa(val, mode, ndigits, &decpt,
						   &sign, &rve);
				int mant_len = rve - buf_r;
				char tmp[mant_len + 1];
				char *s = tmp;
				int pos = 0;

				if (sign)
					tmp[pos++] = '-';
				else if (flags & FLAG_PLUS)
					tmp[pos++] = '+';
				else if (flags & FLAG_SPACE)
					tmp[pos++] = ' ';

				for (int i = 0; i < mant_len; i++)
					tmp[pos++] = buf_r[i];

				tmp[pos] = '\0';
				free(buf_r);
				l = strlen(s);

				break;
			}

			case 'a':
			case 'A': {
				long double val;

				if (length == LENGTH_CAPL)
					val = va_arg(ap, long double);
				else
					val = va_arg(ap, double);

				int upper = (*ptr == 'A');

				if (val == 0.0) {
					char zero_buf[6];
					int pos = 0;
					if (val < 0)
						zero_buf[pos++] = '-';
					else if (flags & FLAG_PLUS)
						zero_buf[pos++] = '+';
					else if (flags & FLAG_SPACE)
						zero_buf[pos++] = ' ';
					zero_buf[pos++] = '0';
					zero_buf[pos++] = 'x';
					zero_buf[pos++] = '0';
					zero_buf[pos++] = upper ? 'P' : 'p';
					zero_buf[pos++] = '0';
					zero_buf[pos] = '\0';
					s = zero_buf;
					l = pos;
					break;
				}

				int exp;
				double abs_val = (val < 0) ? -val : val;
				double mant = frexp(abs_val, &exp);

				unsigned long long hm =
					(unsigned long long)(mant *
							     (1ULL << 53));

				int pos = 0;
				if (val < 0)
					buf[pos++] = '-';
				else if (flags & FLAG_PLUS)
					buf[pos++] = '+';
				else if (flags & FLAG_SPACE)
					buf[pos++] = ' ';

				buf[pos++] = '0';
				buf[pos++] = upper ? 'X' : 'x';

				pos += utoa_base(hm, buf + pos, 16,
						 precision > 0 ? precision : 0,
						 upper);

				buf[pos++] = upper ? 'P' : 'p';
				pos += utoa_base(exp - 53, buf + pos, 10, 0, 0);

				buf[pos] = '\0';
				s = buf;
				l = pos;
				break;
			}

			case 'C':
				length = LENGTH_L;
				/* fallthrough */
			case 'c': {
				if (length == LENGTH_L) {
					// wint_t wc = va_arg(ap, wint_t);
					// int n = wcrtomb(buf, wc, NULL);
					// s = buf;
					// l = n;
				} else {
					char c = (char)va_arg(ap, int);
					buf[0] = c;
					buf[1] = '\0';
					s = buf;
					l = 1;
				}
				break;
			}

			case 'S':
				length = LENGTH_L;
				/* fallthrough */
			case 's':
				if (length == LENGTH_L) {
					// wchar_t *wstr = va_arg(ap, wchar_t *);
					// char buf[1024];
					// l = wcstombs(buf, wstr, sizeof(buf));
					// s = buf;
				} else {
					s = va_arg(ap, char *);
					l = strlen(s);
					if (precision >= 0 && precision < l)
						l = precision;
				}
				break;

			case 'p': {
				void *ptr_val = va_arg(ap, void *);
				uintptr_t uptr = (uintptr_t)ptr_val;
				int len = utoa_base((unsigned long long)uptr,
						    buf + 2, 16,
						    sizeof(void *) * 2, 0);
				buf[0] = '0';
				buf[1] = 'x';
				buf[len + 2] = '\0';
				l = len + 2;
				s = buf;
				break;
			}

			case 'n': {
				int *ip = va_arg(ap, int *);
				if (ip != NULL) {
					*ip = total_printed;
				}
				l = 0;
				break;
			}
			case '%': {
				char c = '%';
				s = &c;
				l = 1;
				break;
			}
			default:
				errno = EINVAL;
				return -1;
			}

			ptr++;

			if (l < width) {
				padding = width - l;
			}

			char pad_char = ' ';
			if ((flags & FLAG_ZERO) && !(flags & FLAG_MINUS) &&
			    (*ptr == 'd' || *ptr == 'i' || *ptr == 'u' ||
			     *ptr == 'o' || *ptr == 'x' || *ptr == 'X') &&
			    precision < 0) {
				pad_char = '0';
			}

			if ((flags & FLAG_MINUS) == 0) {
				for (int i = 0; i < padding; i++) {
					if (pad_char == '0') {
						fwrite("0", 1, 1, stream);
					} else {
						fwrite(" ", 1, 1, stream);
					}
					total_printed++;
				}
			}

			if (l > 0) {
				if (s != NULL)
					fwrite(s, 1, l, stream);
				total_printed += l;
			}

			if (flags & FLAG_MINUS) {
				for (int i = 0; i < padding; i++) {
					fwrite(" ", 1, 1, stream);
					total_printed++;
				}
			}
		}
	}

	return total_printed;
}
