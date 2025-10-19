#ifndef __TIME_H
#define __TIME_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#define CLOCKS_PER_SEC 1000000L
#define TIME_UTC       1

#define CLOCK_REALTIME		 0
#define CLOCK_MONOTONIC		 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID	 3

#define TIMER_ABSTIME 0x01

typedef __INT64_TYPE__ time_t;
typedef __INT32_TYPE__ clockid_t;
typedef __INT64_TYPE__ clock_t;
typedef __SIZE_TYPE__ size_t;
typedef __INT32_TYPE__ clockid_t;
typedef __INT64_TYPE__ pid_t;

extern int daylight;
extern long timezone;
extern char *tzname[];

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	long tm_gmtoff;
	const char *tm_zone;
};

char *asctime(const struct tm *);
clock_t clock(void);
int clock_getcpuclockid(pid_t, clockid_t *);
int clock_getres(clockid_t, struct timespec *);
int clock_gettime(clockid_t, struct timespec *);
int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *);
int clock_settime(clockid_t, const struct timespec *);
char *ctime(const time_t *);
double difftime(time_t, time_t);
struct tm *getdate(const char *);
struct tm *gmtime(const time_t *);
struct tm *gmtime_r(const time_t *restrict, struct tm *restrict);
struct tm *localtime(const time_t *);
struct tm *localtime_r(const time_t *restrict, struct tm *restrict);
time_t mktime(struct tm *);
int nanosleep(const struct timespec *, struct timespec *);
size_t strftime(char *restrict, size_t, const char *restrict,
		const struct tm *restrict);
// TODO:
// size_t strftime_l(char *restrict, size_t, const char *restrict,
// 		  const struct tm *restrict, locale_t);
char *strptime(const char *restrict, const char *restrict, struct tm *restrict);
time_t time(time_t *);
// TODO:
// int timer_create(clockid_t, struct sigevent *restrict, timer_t *restrict);
// int timer_delete(timer_t);
// int timer_getoverrun(timer_t);
// int timer_gettime(timer_t, struct itimerspec *);
// int timer_settime(timer_t, int, const struct itimerspec *restrict,
// 		  struct itimerspec *restrict);
int timespec_get(struct timespec *, int);
void tzset(void);

#endif
