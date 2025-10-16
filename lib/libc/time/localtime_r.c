#include <time.h>

struct tm *localtime_r(const time_t *restrict timer, struct tm *restrict result)
{
	time_t local = *timer - timezone;
	return gmtime_r(&local, result);
}
