#include <time.h>

struct tm *localtime(const time_t *timer)
{
	static struct tm result;
	return localtime_r(timer, &result);
}
