// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/assert.h.html

#include "core.h"

#define NDEBUG
#include <assert.h>
#ifndef assert
#error
#endif

#undef NDEBUG
#include <assert.h>
#ifndef assert
#error
#endif
