#include <time.h>
#include <string.h>

static size_t append_string(char *restrict *s, size_t *remaining,
			    const char *str)
{
	size_t len = strlen(str);
	if (len >= *remaining) {
		return 0;
	}
	strcpy(*s, str);
	*s += len;
	*remaining -= len;
	return len;
}

static size_t append_char(char *restrict *s, size_t *remaining, char c)
{
	if (*remaining <= 1) {
		return 0;
	}
	**s = c;
	(*s)++;
	(*remaining)--;
	return 1;
}

static size_t format_int(char *restrict *s, size_t *remaining, int value,
			 int width, char pad, int show_sign)
{
	char buffer[32];
	char *ptr = buffer + sizeof(buffer) - 1;
	*ptr = '\0';

	int negative = 0;
	if (value < 0) {
		negative = 1;
		value = -value;
	}

	do {
		*--ptr = '0' + (value % 10);
		value /= 10;
	} while (value > 0);

	if (negative) {
		*--ptr = '-';
	} else if (show_sign && value >= 0) {
		*--ptr = '+';
	}

	int len = (buffer + sizeof(buffer) - 1) - ptr;

	while (len < width) {
		*--ptr = pad;
		len++;
	}

	return append_string(s, remaining, ptr);
}

static const char *weekday_abbr[] = { "Sun", "Mon", "Tue", "Wed",
				      "Thu", "Fri", "Sat" };
static const char *weekday_full[] = { "Sunday",	   "Monday",   "Tuesday",
				      "Wednesday", "Thursday", "Friday",
				      "Saturday" };

static const char *month_abbr[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
				    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char *month_full[] = { "January", "February", "March",
				    "April",   "May",	   "June",
				    "July",    "August",   "September",
				    "October", "November", "December" };

static int day_of_year(const struct tm *tm)
{
	static const int days_to_month[] = { 0,	  31,  59,  90,	 120, 151,
					     181, 212, 243, 273, 304, 334 };
	int days = days_to_month[tm->tm_mon] + tm->tm_mday;

	if (tm->tm_mon > 1) {
		int year = tm->tm_year + 1900;
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			days++;
		}
	}
	return days;
}

static int iso_week_number(const struct tm *tm, int *week_year)
{
	int year = tm->tm_year + 1900;
	int yday = tm->tm_yday + 1;
	int wday = tm->tm_wday;
	if (wday == 0)
		wday = 7;

	int jan4_wday = (4 + year + (year - 1) / 4 - (year - 1) / 100 +
			 (year - 1) / 400) %
			7;
	if (jan4_wday == 0)
		jan4_wday = 7;

	int week1_start = 4 - jan4_wday + 1;

	int week = (yday - week1_start + 7) / 7;

	if (week < 1) {
		*week_year = year - 1;

		int prev_jan4_wday = (jan4_wday - 1 + 7) % 7;
		if (prev_jan4_wday == 0)
			prev_jan4_wday = 7;
		int prev_week1_start = 4 - prev_jan4_wday + 1;
		int prev_year_days = 365;
		if (((year - 1) % 4 == 0 && (year - 1) % 100 != 0) ||
		    ((year - 1) % 400 == 0)) {
			prev_year_days = 366;
		}
		week = (prev_year_days - prev_week1_start + 8) / 7;
	} else {
		*week_year = year;

		int year_days = 365;
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
			year_days = 366;
		}
		if (week1_start + (week - 1) * 7 > year_days - 3) {
			if (yday > year_days - (7 - jan4_wday)) {
				*week_year = year + 1;
				week = 1;
			}
		}
	}

	return week;
}

size_t strftime(char *restrict s, size_t maxsize, const char *restrict format,
		const struct tm *restrict timeptr)
{
	if (maxsize == 0)
		return 0;

	char *orig_s = s;
	size_t remaining = maxsize - 1;
	const char *ptr = format;

	while (*ptr && remaining > 0) {
		if (*ptr != '%') {
			if (!append_char(&s, &remaining, *ptr)) {
				*orig_s = '\0';
				return 0;
			}
			ptr++;
			continue;
		}

		ptr++;
		if (!*ptr)
			break;

		char pad_char = ' ';
		int show_sign = 0;
		int min_width = 0;

		if (*ptr == '0') {
			pad_char = '0';
			ptr++;
		} else if (*ptr == '+') {
			pad_char = '0';
			show_sign = 1;
			ptr++;
		}

		while (*ptr >= '0' && *ptr <= '9') {
			min_width = min_width * 10 + (*ptr - '0');
			ptr++;
		}

		if (*ptr == 'E' || *ptr == 'O') {
			ptr++;
		}

		switch (*ptr) {
		case 'a':
			if (!append_string(&s, &remaining,
					   weekday_abbr[timeptr->tm_wday])) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'A':
			if (!append_string(&s, &remaining,
					   weekday_full[timeptr->tm_wday])) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'b':
		case 'h':
			if (!append_string(&s, &remaining,
					   month_abbr[timeptr->tm_mon])) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'B':
			if (!append_string(&s, &remaining,
					   month_full[timeptr->tm_mon])) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'c':
			if (!append_string(&s, &remaining,
					   weekday_abbr[timeptr->tm_wday]) ||
			    !append_char(&s, &remaining, ' ') ||
			    !append_string(&s, &remaining,
					   month_abbr[timeptr->tm_mon]) ||
			    !append_char(&s, &remaining, ' ') ||
			    !format_int(&s, &remaining, timeptr->tm_mday, 2,
					' ', 0) ||
			    !append_char(&s, &remaining, ' ') ||
			    !format_int(&s, &remaining, timeptr->tm_hour, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_sec, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ' ') ||
			    !format_int(&s, &remaining, timeptr->tm_year + 1900,
					4, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'C':
			if (!format_int(&s, &remaining,
					(timeptr->tm_year + 1900) / 100,
					min_width ? min_width : 2, pad_char,
					show_sign)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'd':
			if (!format_int(&s, &remaining, timeptr->tm_mday, 2,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'D':
			if (!format_int(&s, &remaining, timeptr->tm_mon + 1, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, '/') ||
			    !format_int(&s, &remaining, timeptr->tm_mday, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, '/') ||
			    !format_int(&s, &remaining, timeptr->tm_year % 100,
					2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'e':
			if (!format_int(&s, &remaining, timeptr->tm_mday, 2,
					' ', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'F': {
			int width = min_width ? min_width - 6 : 4;
			if (width < 4)
				width = 4;
			if (!format_int(&s, &remaining, timeptr->tm_year + 1900,
					width, pad_char, show_sign) ||
			    !append_char(&s, &remaining, '-') ||
			    !format_int(&s, &remaining, timeptr->tm_mon + 1, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, '-') ||
			    !format_int(&s, &remaining, timeptr->tm_mday, 2,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'g': {
			int week_year;
			iso_week_number(timeptr, &week_year);
			if (!format_int(&s, &remaining, week_year % 100, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'G': {
			int week_year;
			iso_week_number(timeptr, &week_year);
			if (!format_int(&s, &remaining, week_year,
					min_width ? min_width : 4, pad_char,
					show_sign)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'H':
			if (!format_int(&s, &remaining, timeptr->tm_hour, 2,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'I': {
			int hour12 = timeptr->tm_hour % 12;
			if (hour12 == 0)
				hour12 = 12;
			if (!format_int(&s, &remaining, hour12, 2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'j':
			if (!format_int(&s, &remaining, timeptr->tm_yday + 1, 3,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'm':
			if (!format_int(&s, &remaining, timeptr->tm_mon + 1, 2,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'M':
			if (!format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'n':
			if (!append_char(&s, &remaining, '\n')) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'p':
			if (!append_string(&s, &remaining,
					   timeptr->tm_hour < 12 ? "AM" :
								   "PM")) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'r': {
			int hour12 = timeptr->tm_hour % 12;
			if (hour12 == 0)
				hour12 = 12;
			if (!format_int(&s, &remaining, hour12, 2, '0', 0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_sec, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ' ') ||
			    !append_string(&s, &remaining,
					   timeptr->tm_hour < 12 ? "AM" :
								   "PM")) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'R':
			if (!format_int(&s, &remaining, timeptr->tm_hour, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 's':

			if (!append_char(&s, &remaining, '0')) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'S':
			if (!format_int(&s, &remaining, timeptr->tm_sec, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 't':
			if (!append_char(&s, &remaining, '\t')) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'T':
			if (!format_int(&s, &remaining, timeptr->tm_hour, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_sec, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'u': {
			int wday = timeptr->tm_wday;
			if (wday == 0)
				wday = 7;
			if (!format_int(&s, &remaining, wday, 1, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'U': {
			int week =
				(timeptr->tm_yday + 7 - timeptr->tm_wday) / 7;
			if (!format_int(&s, &remaining, week, 2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'V': {
			int week_year;
			int week = iso_week_number(timeptr, &week_year);
			if (!format_int(&s, &remaining, week, 2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'w':
			if (!format_int(&s, &remaining, timeptr->tm_wday, 1,
					'0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'W': {
			int wday = timeptr->tm_wday;
			if (wday == 0)
				wday = 7;
			int week = (timeptr->tm_yday + 7 - wday + 1) / 7;
			if (!format_int(&s, &remaining, week, 2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'x':
			if (!format_int(&s, &remaining, timeptr->tm_mon + 1, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, '/') ||
			    !format_int(&s, &remaining, timeptr->tm_mday, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, '/') ||
			    !format_int(&s, &remaining, timeptr->tm_year % 100,
					2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'X':
			if (!format_int(&s, &remaining, timeptr->tm_hour, 2,
					'0', 0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_min, 2, '0',
					0) ||
			    !append_char(&s, &remaining, ':') ||
			    !format_int(&s, &remaining, timeptr->tm_sec, 2, '0',
					0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'y':
			if (!format_int(&s, &remaining, timeptr->tm_year % 100,
					2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'Y':
			if (!format_int(&s, &remaining, timeptr->tm_year + 1900,
					min_width ? min_width : 4, pad_char,
					show_sign)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		case 'z': {
			if (timeptr->tm_isdst < 0) {
				break;
			}
			long offset = timeptr->tm_gmtoff;
			char sign = '+';
			if (offset < 0) {
				sign = '-';
				offset = -offset;
			}
			int hours = offset / 3600;
			int minutes = (offset % 3600) / 60;
			if (!append_char(&s, &remaining, sign) ||
			    !format_int(&s, &remaining, hours, 2, '0', 0) ||
			    !format_int(&s, &remaining, minutes, 2, '0', 0)) {
				*orig_s = '\0';
				return 0;
			}
		} break;
		case 'Z':
			if (timeptr->tm_zone) {
				if (!append_string(&s, &remaining,
						   timeptr->tm_zone)) {
					*orig_s = '\0';
					return 0;
				}
			}
			break;
		case '%':
			if (!append_char(&s, &remaining, '%')) {
				*orig_s = '\0';
				return 0;
			}
			break;
		default:

			if (!append_char(&s, &remaining, '%') ||
			    !append_char(&s, &remaining, *ptr)) {
				*orig_s = '\0';
				return 0;
			}
			break;
		}
		ptr++;
	}

	*s = '\0';
	return s - orig_s;
}
