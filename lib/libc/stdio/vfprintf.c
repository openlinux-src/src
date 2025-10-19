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
#include <wchar.h>

extern char *dtoa(double, int mode, int ndigits, int *decpt, int *sign,
		  char **rve);
extern void freedtoa(char *s);

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

			ptr++;

			enum {
				FLAG_NONE = 0,
				FLAG_MINUS = 1 << 0,
				FLAG_PLUS = 1 << 1,
				FLAG_HASH = 1 << 2,
				FLAG_ZERO = 1 << 3,
				FLAG_SPACE = 1 << 4
			} flags = FLAG_NONE;

			enum {
				LENGTH_NONE,
				LENGTH_HH,
				LENGTH_H,
				LENGTH_L,
				LENGTH_LL,
				LENGTH_J,
				LENGTH_Z,
				LENGTH_T,
				LENGTH_CAPL
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
					val = va_arg(ap, ssize_t);
					break;
				case LENGTH_T:
					val = va_arg(ap, ptrdiff_t);
					break;
				case LENGTH_CAPL:
					errno = EINVAL;
					return -1;
				}

				unsigned long long uval;

				if (val < 0) {
					uval = (unsigned long long)(-val);
					negative = 1;
				} else {
					uval = (unsigned long long)val;
				}

				l = utoa_base(uval, buf, 10, precision, 0);

				s = buf;
				if (negative) {
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
				char fmt_char = *ptr;

				switch (length) {
				case LENGTH_NONE:
					val = va_arg(ap, unsigned int);
					break;
				case LENGTH_HH:
					val = (unsigned char)va_arg(
						ap, unsigned int);
					break;
				case LENGTH_H:
					val = (unsigned short)va_arg(
						ap, unsigned int);
					break;
				case LENGTH_L:
					val = va_arg(ap, unsigned long);
					break;
				case LENGTH_LL:
					val = va_arg(ap, unsigned long long);
					break;
				case LENGTH_J:
					val = va_arg(ap, uintmax_t);
					break;
				case LENGTH_Z:
					val = va_arg(ap, size_t);
					break;
				case LENGTH_T:
					val = (ptrdiff_t)va_arg(ap, ptrdiff_t);
					break;
				case LENGTH_CAPL:
					errno = EINVAL;
					return -1;
				}

				if (fmt_char == 'o') {
					l = utoa_base(val, buf, 8, precision,
						      0);
				} else if (fmt_char == 'u') {
					l = utoa_base(val, buf, 10, precision,
						      0);
				} else if (fmt_char == 'x') {
					l = utoa_base(val, buf, 16, precision,
						      0);
				} else if (fmt_char == 'X') {
					l = utoa_base(val, buf, 16, precision,
						      1);
				} else {
					l = utoa_base(val, buf, 10, precision,
						      0);
				}

				s = buf;

				if ((flags & FLAG_HASH) && val != 0) {
					if (fmt_char == 'o') {
						memmove(s + 1, s, l);
						s[0] = '0';
						s[++l] = '\0';
					} else if (fmt_char == 'x') {
						memmove(s + 2, s, l);
						s[0] = '0';
						s[1] = 'x';
						l += 2;
						s[l] = '\0';
					} else if (fmt_char == 'X') {
						memmove(s + 2, s, l);
						s[0] = '0';
						s[1] = 'X';
						l += 2;
						s[l] = '\0';
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
				double val;

				if (length == LENGTH_CAPL)
					val = (double)va_arg(ap, long double);
				else
					val = va_arg(ap, double);

				if (precision == -1)
					precision = 6;

				s = buf;
				int pos = 0;

				if (isnan(val)) {
					strcpy(buf,
					       (*ptr == 'F' || *ptr == 'E' ||
						*ptr == 'G') ?
						       "NAN" :
						       "nan");
					l = 3;
					break;
				}
				if (isinf(val)) {
					if (val < 0) {
						buf[pos++] = '-';
					} else if (flags & FLAG_PLUS) {
						buf[pos++] = '+';
					} else if (flags & FLAG_SPACE) {
						buf[pos++] = ' ';
					}
					strcpy(buf + pos,
					       (*ptr == 'F' || *ptr == 'E' ||
						*ptr == 'G') ?
						       "INF" :
						       "inf");
					l = pos + 3;
					break;
				}

				if (val < 0) {
					buf[pos++] = '-';
					val = -val;
				} else if (flags & FLAG_PLUS) {
					buf[pos++] = '+';
				} else if (flags & FLAG_SPACE) {
					buf[pos++] = ' ';
				}

				switch (*ptr) {
				case 'f':
				case 'F': {
					int decpt, sign;
					char *rve;
					char *digits = dtoa(val, 3, precision,
							    &decpt, &sign,
							    &rve);

					if (sign && val != 0.0) {
						buf[pos++] = '-';
					}

					if (decpt <= 1 && digits[0] == '0' &&
					    digits[1] == '\0') {
						buf[pos++] = '0';
						if (precision > 0) {
							buf[pos++] = '.';
							for (int i = 0;
							     i < precision;
							     i++) {
								buf[pos++] =
									'0';
							}
						}
					} else if (decpt <= 0) {
						buf[pos++] = '0';
						if (precision > 0) {
							buf[pos++] = '.';
							for (int i = 0;
							     i < -decpt &&
							     i < precision;
							     i++) {
								buf[pos++] =
									'0';
							}
							int remaining =
								precision +
								decpt;
							char *d = digits;
							while (*d &&
							       remaining-- >
								       0) {
								buf[pos++] =
									*d++;
							}
							while (remaining-- >
							       0) {
								buf[pos++] =
									'0';
							}
						}
					} else {
						char *d = digits;
						int digits_used = 0;
						for (int i = 0; i < decpt && *d;
						     i++) {
							buf[pos++] = *d++;
							digits_used++;
						}
						while (digits_used < decpt) {
							buf[pos++] = '0';
							digits_used++;
						}
						if (precision > 0) {
							buf[pos++] = '.';
							for (int i = 0;
							     i < precision;
							     i++) {
								if (*d) {
									buf[pos++] =
										*d++;
								} else {
									buf[pos++] =
										'0';
								}
							}
						}
					}

					buf[pos] = '\0';
					l = pos;
					freedtoa(digits);
					break;
				}
				case 'e':
				case 'E': {
					int decpt, sign;
					char *rve;
					char *digits =
						dtoa(val, 2, precision + 1,
						     &decpt, &sign, &rve);

					if (sign && val != 0.0) {
						buf[pos++] = '-';
					}

					if (*digits) {
						buf[pos++] = *digits++;
					} else {
						buf[pos++] = '0';
					}

					if (precision > 0) {
						buf[pos++] = '.';
						for (int i = 0; i < precision;
						     i++) {
							if (*digits) {
								buf[pos++] =
									*digits++;
							} else {
								buf[pos++] =
									'0';
							}
						}
					}

					buf[pos++] = (*ptr == 'E') ? 'E' : 'e';
					int exp = decpt - 1;
					buf[pos++] = (exp >= 0) ? '+' : '-';
					if (exp < 0)
						exp = -exp;
					if (exp < 10)
						buf[pos++] = '0';
					pos += utoa_base(exp, buf + pos, 10, 0,
							 0);

					buf[pos] = '\0';
					l = pos;
					freedtoa(digits);
					break;
				}
				case 'g':
				case 'G': {
					int decpt, sign;
					char *rve;
					char *digits = dtoa(val, 2, precision,
							    &decpt, &sign,
							    &rve);

					if (sign && val != 0.0) {
						buf[pos++] = '-';
					}

					int exp = decpt - 1;
					if (exp < -4 || exp >= precision) {
						if (*digits) {
							buf[pos++] = *digits++;
						}
						if (*digits) {
							buf[pos++] = '.';
							while (*digits) {
								buf[pos++] =
									*digits++;
							}
						}
						buf[pos++] = (*ptr == 'G') ?
								     'E' :
								     'e';
						buf[pos++] = (exp >= 0) ? '+' :
									  '-';
						if (exp < 0)
							exp = -exp;
						if (exp < 10)
							buf[pos++] = '0';
						pos += utoa_base(exp, buf + pos,
								 10, 0, 0);
					} else {
						if (decpt <= 0) {
							buf[pos++] = '0';
							if (*digits &&
							    *digits != '0') {
								buf[pos++] =
									'.';
								for (int i = 0;
								     i < -decpt;
								     i++) {
									buf[pos++] =
										'0';
								}
								char *d =
									digits;
								while (*d &&
								       *d != '0') {
									buf[pos++] =
										*d++;
								}
							}
						} else {
							char *d = digits;
							for (int i = 0;
							     i < decpt && *d;
							     i++) {
								buf[pos++] =
									*d++;
							}
							if (*d) {
								buf[pos++] =
									'.';
								while (*d) {
									buf[pos++] =
										*d++;
								}
							}
						}
					}

					while (pos > 1 && buf[pos - 1] == '0' &&
					       pos > 2 && buf[pos - 2] != 'e' &&
					       buf[pos - 2] != 'E') {
						pos--;
					}
					if (pos > 1 && buf[pos - 1] == '.') {
						pos--;
					}

					buf[pos] = '\0';
					l = pos;
					freedtoa(digits);
					break;
				}
				}

				break;
			}

			case 'a':
			case 'A': {
				double val;

				if (length == LENGTH_CAPL)
					val = (double)va_arg(ap, long double);
				else
					val = va_arg(ap, double);

				int upper = (*ptr == 'A');
				int pos = 0;

				if (val < 0) {
					buf[pos++] = '-';
					val = -val;
				} else if (flags & FLAG_PLUS) {
					buf[pos++] = '+';
				} else if (flags & FLAG_SPACE) {
					buf[pos++] = ' ';
				}

				if (val == 0.0) {
					strcpy(buf + pos,
					       upper ? "0X0P+0" : "0x0p+0");
					l = pos + 6;
					break;
				}

				int exp;
				double mant = frexp(val, &exp);
				mant *= 2.0;
				exp--;

				buf[pos++] = '0';
				buf[pos++] = upper ? 'X' : 'x';
				buf[pos++] = '1';

				if (precision == -1) {
					precision = 13;
				}

				if (precision > 0) {
					buf[pos++] = '.';
					mant -= 1.0;
					for (int i = 0; i < precision; i++) {
						mant *= 16.0;
						int digit = (int)mant;
						if (digit > 15)
							digit = 15;
						buf[pos++] =
							(digit < 10) ?
								('0' + digit) :
								(upper ? ('A' +
									  digit -
									  10) :
									 ('a' +
									  digit -
									  10));
						mant -= digit;
					}

					while (pos > 0 && buf[pos - 1] == '0') {
						pos--;
					}
					if (pos > 0 && buf[pos - 1] == '.') {
						pos--;
					}
				}

				buf[pos++] = upper ? 'P' : 'p';
				if (exp >= 0) {
					buf[pos++] = '+';
				} else {
					buf[pos++] = '-';
					exp = -exp;
				}
				pos += utoa_base(exp, buf + pos, 10, 0, 0);

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
				buf[0] = '%';
				s = buf;
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

			char format_char = *(ptr - 1);
			char pad_char = ' ';
			if ((flags & FLAG_ZERO) && !(flags & FLAG_MINUS) &&
			    (format_char == 'd' || format_char == 'i' ||
			     format_char == 'u' || format_char == 'o' ||
			     format_char == 'x' || format_char == 'X' ||
			     format_char == 'f' || format_char == 'F' ||
			     format_char == 'e' || format_char == 'E' ||
			     format_char == 'g' || format_char == 'G') &&
			    precision < 0) {
				pad_char = '0';
			}

			if ((flags & FLAG_MINUS) == 0) {
				if (pad_char == '0' && s != NULL) {
					int prefix_len = 0;

					if (s[0] == '-' || s[0] == '+' ||
					    s[0] == ' ') {
						fwrite(s, 1, 1, stream);
						total_printed++;
						s++;
						l--;
						prefix_len++;
					}

					if (l >= 2 && s[0] == '0' &&
					    (s[1] == 'x' || s[1] == 'X')) {
						fwrite(s, 1, 2, stream);
						total_printed += 2;
						s += 2;
						l -= 2;
						prefix_len += 2;
					}

					for (int i = 0; i < padding; i++) {
						fwrite("0", 1, 1, stream);
						total_printed++;
					}
				} else {
					for (int i = 0; i < padding; i++) {
						if (pad_char == '0') {
							fwrite("0", 1, 1,
							       stream);
						} else {
							fwrite(" ", 1, 1,
							       stream);
						}
						total_printed++;
					}
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
