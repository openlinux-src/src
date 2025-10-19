#ifndef __STDINT_H__
#define __STDINT_H__

#undef WCHAR_MAX
#undef WCHAR_MIN
#if L'\0' - 1 > 0
#define WCHAR_MAX (0xffffffffu + L'\0')
#define WCHAR_MIN (0 + L'\0')
#else
#define WCHAR_MAX (0x7fffffff + L'\0')
#define WCHAR_MIN (-1 - 0x7fffffff + L'\0')
#endif

#define INT8_MIN  (-1 - 0x7f)
#define INT16_MIN (-1 - 0x7fff)
#define INT32_MIN (-1 - 0x7fffffff)
#define INT64_MIN (-1 - 0x7fffffffffffffff)

#define INT8_MAX  (0x7f)
#define INT16_MAX (0x7fff)
#define INT32_MAX (0x7fffffff)
#define INT64_MAX (0x7fffffffffffffff)

#define UINT8_MAX  (0xff)
#define UINT16_MAX (0xffff)
#define UINT32_MAX (0xffffffffu)
#define UINT64_MAX (0xffffffffffffffffu)

#define INT_FAST8_MIN  INT8_MIN
#define INT_FAST64_MIN INT64_MIN

#define INT_LEAST8_MIN	INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_FAST8_MAX  INT8_MAX
#define INT_FAST64_MAX INT64_MAX

#define INT_LEAST8_MAX	INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_FAST8_MAX	UINT8_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define UINT_LEAST8_MAX	 UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INTMAX_MIN  INT64_MIN
#define INTMAX_MAX  INT64_MAX
#define UINTMAX_MAX UINT64_MAX

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;

typedef __INT8_TYPE__ int_least8_t;
typedef __INT16_TYPE__ int_least16_t;
typedef __INT32_TYPE__ int_least32_t;
typedef __INT64_TYPE__ int_least64_t;
typedef __UINT8_TYPE__ uint_least8_t;
typedef __UINT16_TYPE__ uint_least16_t;
typedef __UINT32_TYPE__ uint_least32_t;
typedef __UINT64_TYPE__ uint_least64_t;

typedef __INTPTR_TYPE__ int_fast8_t;
typedef __INTPTR_TYPE__ int_fast16_t;
typedef __INTPTR_TYPE__ int_fast32_t;
typedef __INTPTR_TYPE__ int_fast64_t;
typedef __UINTPTR_TYPE__ uint_fast8_t;
typedef __UINTPTR_TYPE__ uint_fast16_t;
typedef __UINTPTR_TYPE__ uint_fast32_t;
typedef __UINTPTR_TYPE__ uint_fast64_t;

typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#endif
