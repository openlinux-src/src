#include <stdbool.h>
#include <float.h>
#include "__complex.h"

#define THRESH	     (LDBL_MAX / 2.414213562373095048801688724209698L)
#define cpackl(r, i) ((r) + (i) * (long double complex)I)

long double complex csqrtl(long double complex z)
{
	long double complex result;
	long double a, b;
	long double t;
	bool scale;
	a = creall(z);
	b = cimagl(z);
	/* Handle special cases. */
	if (z == 0.0L)
		return (cpackl((long double)0.0L, b));
	if (isinf(b))
		return (cpackl((long double)INFINITY, b));
	if (isnan(a)) {
		t = (b - b) / (b - b); /* raise invalid if b is not a NaN */
		return (cpackl(a, t)); /* return NaN + NaN i */
	}
	if (isinf(a)) {
		/*
		 * csqrt(inf + NaN i)  = inf +  NaN i
		 * csqrt(inf + y i)    = inf +  0 i
		 * csqrt(-inf + NaN i) = NaN +- inf i
		 * csqrt(-inf + y i)   = 0   +  inf i
		 */
		if (signbit(a))
			return (cpackl(fabsl(b - b), copysignl(a, b)));
		else
			return (cpackl(a, copysignl(b - b, b)));
	}
	/*
	 * The remaining special case (b is NaN) is handled just fine by
	 * the normal code path below.
	 */
	/* Scale to avoid overflow. */
	if (fabsl(a) >= THRESH || fabsl(b) >= THRESH) {
		a *= 0.25L;
		b *= 0.25L;
		scale = true;
	} else {
		scale = false;
	}
	/* Algorithm 312, CACM vol 10, Oct 1967. */
	if (a >= 0L) {
		t = sqrtl((a + hypotl(a, b)) * 0.5L);
		result = cpackl(t, b / (2.0L * t));
	} else {
		t = sqrtl((-a + hypotl(a, b)) * 0.5L);
		result = cpackl(fabsl(b) / (2.0L * t), copysignl(t, b));
	}
	/* Rescale. */
	if (scale)
		return (result * 2.0L);
	else
		return (result);
}
