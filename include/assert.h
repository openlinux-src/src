#ifndef __ASSERT_H
#define __ASSERT_H

#ifdef NDEBUG
#define assert(ignore) ((void)0)
#else

/**
 * @def assert(expr)
 * @brief Macro to perform runtime assertions.
 *
 * The assert() macro shall insert diagnostics into programs; it shall expand to a void expression.
 * When it is executed, if expression (which shall have a scalar type) is false (that is, compares equal to 0),
 * assert() shall write information about the particular call that failed on stderr and shall call abort().
 * The information written about the call that failed shall include the text of the argument, the name of the source file,
 * the source file line number, and the name of the enclosing function; the latter are, respectively, the values of the
 * preprocessing  * macros __FILE__ and __LINE__ and of the identifier __func__. Forcing a definition of the name NDEBUG, either
 * from the compiler command line or with the preprocessor control statement #define NDEBUG ahead of the #include <assert.h> statement,
 * shall stop assertions from being compiled into the program.
 *
 * @param expr The expression to be evaluated.
 *
 * @see abort
 */
#define assert(__expr)     \
	((__expr) ?        \
		 (void)0 : \
		 __assert(__FILE__, __LINE__, __PRETTY_FUNCTION__, #__expr))
#endif

_Noreturn void __assert(const char *_Nonnull __file, int __line,
			const char *_Nonnull __func,
			const char *_Nonnull __expr);

#endif
