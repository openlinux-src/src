#include <io.h>
#include <stdio.h>
#include <string.h>

enum buf_t { BUF_NORMAL, BUF_DYNAMIC };

static int iota(unsigned long long v, char *s, int b, int p, int u)
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

int ftoa(long double value, char *str, char spec, int precision)
{
	if (value != value) {
		str[0] = 'n';
		str[1] = 'a';
		str[2] = 'n';
		str[3] = '\0';
		return 3;
	}
	if (value > 1.0L / 0.0L) {
		str[0] = 'i';
		str[1] = 'n';
		str[2] = 'f';
		str[3] = '\0';
		return 3;
	}
	if (value < -1.0L / 0.0L) {
		str[0] = '-';
		str[1] = 'i';
		str[2] = 'n';
		str[3] = 'f';
		str[4] = '\0';
		return 4;
	}

	char *s = str;
	int len = 0;

	if (value < 0) {
		*s++ = '-';
		value = -value;
		len++;
	}

	if (precision < 0)
		precision = 6;

	unsigned long long int_part = (unsigned long long)value;
	long double frac_part = value - (long double)int_part;

	char intbuf[32];
	int intlen = 0;
	if (int_part == 0) {
		intbuf[intlen++] = '0';
	} else {
		while (int_part) {
			intbuf[intlen++] = '0' + (int_part % 10);
			int_part /= 10;
		}
	}
	for (int i = intlen - 1; i >= 0; --i) {
		*s++ = intbuf[i];
		len++;
	}

	if (precision > 0 || spec == 'f') {
		*s++ = '.';
		len++;
		for (int i = 0; i < precision; ++i) {
			frac_part *= 10.0L;
			int digit = (int)frac_part;
			*s++ = '0' + digit;
			frac_part -= digit;
			len++;
		}
	}

	*s = '\0';
	return len;
}

static int bwrite(enum buf_t btype, char **buf, size_t *bufsz, size_t *buf_pos,
		  const char *s, size_t slen)
{
	if (!buf || !bufsz || !buf_pos || !s)
		return -1;

	size_t avail = *bufsz > *buf_pos ? *bufsz - *buf_pos : 0;

	if (slen >= avail) {
		if (btype == BUF_DYNAMIC) {
			size_t new_size = *bufsz ? *bufsz * 2 : 64;
			while (new_size < *buf_pos + slen + 1)
				new_size *= 2;

			char *new_buf = realloc(*buf, new_size);
			if (!new_buf)
				return -1;

			*buf = new_buf;
			*bufsz = new_size;
			avail = new_size - *buf_pos;
		} else {
			slen = avail > 0 ? avail - 1 : 0;
		}
	}

	if (slen > 0) {
		memcpy(*buf + *buf_pos, s, slen);
		*buf_pos += slen;
	}

	if (*buf_pos < *bufsz)
		(*buf)[*buf_pos] = '\0';

	// TODO: flush (?)

	return (int)slen;
}

int __vsnprintf(char **restrict buf, size_t bufsz, const char *restrict fmt,
		va_list ap)
{
	const char *p;
	size_t pos;
	enum buf_t btype;

	p = fmt;
	pos = 0;
	btype = (buf) ? BUF_NORMAL : BUF_DYNAMIC;

	while (*p) {
		if (*p != '%') {
			bwrite(btype, buf, &bufsz, &pos, p, 1);
			p++;
			continue;
		}

		p++;
		if (*p == '%') {
			bwrite(btype, buf, &bufsz, &pos, p, 1);
			p++;
			continue;
		}

		enum {
			FLAG_MINUS = 1 << 0,
			FLAG_PLUS = 1 << 1,
			FLAG_SPACE = 1 << 2,
			FLAG_ZERO = 1 << 3,
			FLAG_HASH = 1 << 4
		} flags;

		while (*p == '-' || *p == '+' || *p == ' ' || *p == '0' ||
		       *p == '#') {
			if (*p == '-')
				flags |= FLAG_MINUS;
			else if (*p == '+')
				flags |= FLAG_PLUS;
			else if (*p == ' ')
				flags |= FLAG_SPACE;
			else if (*p == '0')
				flags |= FLAG_ZERO;
			else if (*p == '#')
				flags |= FLAG_HASH;
			p++;
		}

		int width = 0;
		if (*p == '*') {
			width = va_arg(ap, int);
			if (width < 0) {
				flags |= FLAG_MINUS;
				width = -width;
			}
			p++;
		} else {
			while (*p >= '0' && *p <= '9') {
				width = width * 10 + (*p - '0');
				p++;
			}
		}

		int precision = -1;
		if (*p == '.') {
			p++;
			if (*p == '*') {
				precision = va_arg(ap, int);
				if (precision < 0)
					precision = -1;
				p++;
			} else {
				precision = 0;
				while (*p >= '0' && *p <= '9') {
					precision = precision * 10 + (*p - '0');
					p++;
				}
			}
		}

		enum {
			LEN_NONE,
			LEN_HH,
			LEN_H,
			LEN_L,
			LEN_LL,
			LEN_Z,
			LEN_T,
			LEN_CAPL
		} length = LEN_NONE;

		if (*p == 'h') {
			if (*(p + 1) == 'h') {
				length = LEN_HH;
				p += 2;
			} else {
				length = LEN_H;
				p++;
			}
		} else if (*p == 'l') {
			if (*(p + 1) == 'l') {
				length = LEN_LL;
				p += 2;
			} else {
				length = LEN_L;
				p++;
			}
		} else if (*p == 'z') {
			length = LEN_Z;
			p++;
		} else if (*p == 't') {
			length = LEN_T;
			p++;
		} else if (*p == 'L') {
			length = LEN_CAPL;
			p++;
		}

		char spec = *p++;
		char str[65];
		char *output_str = str;
		int str_len = 0;

		switch (spec) {
		case 'd':
		case 'i': {
			long long val;
			int negative = 0;
			if (length == LEN_HH)
				val = (signed char)va_arg(ap, int);
			else if (length == LEN_H)
				val = (short)va_arg(ap, int);
			else if (length == LEN_L)
				val = va_arg(ap, long);
			else if (length == LEN_LL)
				val = va_arg(ap, long long);
			else
				val = va_arg(ap, int);

			if (val < 0) {
				negative = 1;
				val = -val;
			}
			str_len = iota(val, str, 10, precision, 0);
			if (negative) {
				memmove(str + 1, str, str_len);
				str[0] = '-';
				str_len++;
				str[str_len] = '\0';
			} else if (flags & FLAG_PLUS) {
				memmove(str + 1, str, str_len);
				str[0] = '+';
				str_len++;
				str[str_len] = '\0';
			} else if (flags & FLAG_SPACE) {
				memmove(str + 1, str, str_len);
				str[0] = ' ';
				str_len++;
				str[str_len] = '\0';
			}
			break;
		}
		case 'u':
		case 'o':
		case 'x':
		case 'X': {
			unsigned long long val;
			if (length == LEN_HH)
				val = (unsigned char)va_arg(ap, unsigned int);
			else if (length == LEN_H)
				val = (unsigned short)va_arg(ap, unsigned int);
			else if (length == LEN_L)
				val = va_arg(ap, unsigned long);
			else if (length == LEN_LL)
				val = va_arg(ap, unsigned long long);
			else
				val = va_arg(ap, unsigned int);

			int base = (spec == 'o') ? 8 : (spec == 'u' ? 10 : 16);
			int upper = (spec == 'X');
			str_len = iota(val, str, base, precision, upper);

			if ((flags & FLAG_HASH) && val != 0) {
				if (spec == 'o') {
					memmove(str + 1, str, str_len);
					str[0] = '0';
					str_len++;
					str[str_len] = '\0';
				} else if (spec == 'x' || spec == 'X') {
					memmove(str + 2, str, str_len);
					str[0] = '0';
					str[1] = (upper ? 'X' : 'x');
					str_len += 2;
					str[str_len] = '\0';
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
			if (length == LEN_CAPL)
				val = va_arg(ap, long double);
			else
				val = va_arg(ap, double);
			if (precision < 0)
				precision = 6;
			str_len = ftoa(val, str, spec, precision);
			break;
		}
		case 'c': {
			str[0] = (char)va_arg(ap, int);
			str_len = 1;
			str[str_len] = '\0';
			break;
		}
		case 's': {
			char *s = va_arg(ap, char *);
			if (!s)
				s = "(null)";
			output_str = s;
			str_len = strlen(s);
			if (precision >= 0 && precision < str_len)
				str_len = precision;
			break;
		}
		case 'p': {
			void *ptr = va_arg(ap, void *);
			unsigned long long val = (unsigned long long)ptr;
			str_len = iota(val, str, 16, sizeof(void *) * 2, 0);
			memmove(str + 2, str, str_len);
			str[0] = '0';
			str[1] = 'x';
			str_len += 2;
			str[str_len] = '\0';
			break;
		}
		case 'n': {
			int *out = va_arg(ap, int *);
			*out = pos;
			str_len = 0;
			break;
		}
		default: {
			str[0] = '%';
			str[1] = spec;
			str_len = 2;
			str[str_len] = '\0';
			break;
		}
		}

		int padding = 0;
		if (str_len < width)
			padding = width - str_len;

		char pad_char = ' ';
		if ((flags & FLAG_ZERO) && !(flags & FLAG_MINUS) &&
		    (spec == 'd' || spec == 'i' || spec == 'u' || spec == 'o' ||
		     spec == 'x' || spec == 'X') &&
		    precision < 0) {
			pad_char = '0';
		}

		if (!(flags & FLAG_MINUS)) {
			for (int i = 0; i < padding; i++) {
				bwrite(btype, buf, &bufsz, &pos, &pad_char, 1);
			}
		}

		if (str_len > 0) {
			for (int i = 0; i < str_len; i++) {
				bwrite(btype, buf, &bufsz, &pos, &output_str[i],
				       1);
			}
		}

		if (flags & FLAG_MINUS) {
			for (int i = 0; i < padding; i++) {
				bwrite(btype, buf, &bufsz, &pos, " ", 1);
			}
		}
	}

	return (int)pos;
}
