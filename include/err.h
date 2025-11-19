#ifndef __ERR_H
#define __ERR_H

#include <stdarg.h>

_Noreturn void err(int eval, const char *fmt, ...);
_Noreturn void errx(int eval, const char *fmt, ...);

void warn(const char *fmt, ...);
void warnx(const char *fmt, ...);

_Noreturn void verr(int eval, const char *fmt, va_list args);
_Noreturn void verrx(int eval, const char *fmt, va_list args);

void vwarn(const char *fmt, va_list args);
void vwarnx(const char *fmt, va_list args);

#endif
