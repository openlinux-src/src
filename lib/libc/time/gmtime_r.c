#include <time.h>

struct tm *gmtime_r(const time_t *timer, struct tm *result)
{
	time_t t = *timer;
	int days, rem;
	int year, month;

	rem = t % 86400;
	days = t / 86400;
	if (rem < 0) {
		rem += 86400;
		days--;
	}

	result->tm_hour = rem / 3600;
	rem %= 3600;
	result->tm_min = rem / 60;
	result->tm_sec = rem % 60;
	result->tm_isdst = 0;

	result->tm_wday = (4 + days) % 7;
	if (result->tm_wday < 0)
		result->tm_wday += 7;

	long z = days + 719468;
	long era = (z >= 0 ? z : z - 146096) / 146097;
	long day_of_era = z - era * 146097;
	long yoe = (day_of_era - day_of_era / 1460 + day_of_era / 36524 -
		    day_of_era / 146096) /
		   365;
	year = (int)(yoe + era * 400);
	long doy = day_of_era - (365 * yoe + yoe / 4 - yoe / 100);
	result->tm_yday = (int)doy;

	int mp = (5 * doy + 2) / 153;
	result->tm_mday = (int)(doy - (153 * mp + 2) / 5 + 1);
	result->tm_mon = (mp + 2) % 12;
	result->tm_year = year - 1900 + (mp / 10);

	return result;
}
