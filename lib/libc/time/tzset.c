#include <time.h>
#include <ctype.h>

int daylight = 0;
long timezone = 0;
char *tzname[2] = { "UTC", "UTC" };

void tzset(void)
{
	const char *tz = NULL; // getenv("TZ");

	if (tz == NULL || *tz == '\0') {
		timezone = 0;
		daylight = 0;
		tzname[0] = tzname[1] = "UTC";
		return;
	}

	const char *p = tz;
	char sign = 0;
	int hours = 0, mins = 0;

	static char stdname[8];
	int i = 0;
	while (*p && !isdigit((unsigned char)*p) && *p != '+' && *p != '-' &&
	       i < 7) {
		stdname[i++] = *p++;
	}

	stdname[i] = '\0';
	if (stdname[0])
		tzname[0] = stdname;
	else
		tzname[0] = "LCL";

	if (*p == '+' || *p == '-')
		sign = *p++;

	while (isdigit((unsigned char)*p))
		hours = hours * 10 + (*p++ - '0');

	if (*p == ':') {
		p++;
		while (isdigit((unsigned char)*p))
			mins = mins * 10 + (*p++ - '0');
	}

	int total = hours * 3600 + mins * 60;
	if (sign == '+')
		timezone = -total;
	else if (sign == '-')
		timezone = total;
	else
		timezone = 0;

	daylight = 0;
	tzname[1] = tzname[0];
}
