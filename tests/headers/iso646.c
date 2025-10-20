// https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/iso646.h.html

#include "core.h"

#include <iso646.h>

#define STRINGIFY_(x) #x
#define STRINGIFY(x)  STRINGIFY_(x)

TEST(iso646_h)
{
	MACRO_VALUE_STRING(STRINGIFY(and), "&&");
	MACRO_VALUE_STRING(STRINGIFY(and_eq), "&=");
	MACRO_VALUE_STRING(STRINGIFY(bitand), "&");
	MACRO_VALUE_STRING(STRINGIFY(bitor), "|");
	MACRO_VALUE_STRING(STRINGIFY(compl), "~");
	MACRO_VALUE_STRING(STRINGIFY(not), "!");
	MACRO_VALUE_STRING(STRINGIFY(not_eq), "!=");
	MACRO_VALUE_STRING(STRINGIFY(or), "||");
	MACRO_VALUE_STRING(STRINGIFY(or_eq), "|=");
	MACRO_VALUE_STRING(STRINGIFY(xor), "^");
	MACRO_VALUE_STRING(STRINGIFY(xor_eq), "^=");
}
