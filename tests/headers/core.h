#define FN(__r, ...) __r (*_)(__VA_ARGS__)

#define FUNCTION(__f, __t) \
	{                  \
		__t = __f; \
	}

#define MACRO(__m)                   \
	{                            \
		typeof(__m) v = __m; \
	}

#define MACRO_VALUE(__m, __v) _Static_assert(__m == __v, #__m)

#define MACRO_VALUE_STRING(__m, __v)                                       \
	do {                                                               \
		(void)sizeof(                                              \
			char[(__builtin_strcmp(__m, __v) == 0) ? 1 : -1]); \
	} while (0)

#define MACRO_TYPE(__t, __m) \
	{                    \
		__t v = __m; \
	}

#define TYPE(__t)      \
	{              \
		__t v; \
	}

#define INCOMPLETE_TYPE(__t) \
	{                    \
		__t *v;      \
	}

#define STRUCT_MEMBER(__s, __t, __n) \
	{                            \
		__s s;               \
		__t *_ = &(s.__n);   \
	}

#define STRUCT_MEMBER_ARRAY(__s, __t, __n) \
	{                                  \
		__s s;                     \
		__t *p = &(s.__n[0]);      \
	}

#define STRUCT_MEMBER_FUNCTION_POINTER(__s, __t, __n) \
	{                                             \
		__s s;                                \
		__t = (s.__n);                        \
	}

#define TEST(__name) static void __name(void)
