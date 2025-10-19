#ifndef __LIBC_COMPLEX_H__
#define __LIBC_COMPLEX_H__

#include <math.h>
#include <complex.h>

typedef union {
	float complex z;
	struct {
		float x, y;
	} parts;
} float_complex;

typedef union {
	double complex z;
	struct {
		double x, y;
	} parts;
} double_complex;

typedef union {
	long double complex z;
	struct {
		long double x, y;
	} parts;
} long_double_complex;

#define M_IVLN10 (4.34294481903251816668e-01)
#define M_PIL	 (3.14159265358979323846264338327950280e+00L)

#define REAL_PART(z) ((z).parts.x)
#define IMAG_PART(z) ((z).parts.y)

static const long double DP1 = 3.14159265358979323829596852490908531763125L;
static const long double DP2 = 1.6667485837041756656403424829301998703007e-19L;

#define MACHEP	1.1e-16
#define MACHEPF 3.0e-8f

#ifndef __vax__
static const long double DP3 = 1.8830410776607851167459095484560349402753e-39L;
#define MACHEPL 1.1e-38L
#else
static const long double DP3 = 0L;
#define MACHEPL 1.1e-19L
#endif

static void cchsh(double x, double *c, double *s)
{
	double e, ei;

	if (fabs(x) <= 0.5) {
		*c = cosh(x);
		*s = sinh(x);
	} else {
		e = exp(x);
		ei = 0.5 / e;
		e = 0.5 * e;
		*s = e - ei;
		*c = e + ei;
	}
}

static void cchshl(long double x, long double *c, long double *s)
{
	long double e, ei;

	if (fabsl(x) <= 0.5L) {
		*c = coshl(x);
		*s = sinhl(x);
	} else {
		e = expl(x);
		ei = 0.5L / e;
		e = 0.5L * e;
		*s = e - ei;
		*c = e + ei;
	}
}

static void cchshf(float x, float *c, float *s)
{
	float e, ei;

	if (fabsf(x) <= 0.5f) {
		*c = coshf(x);
		*s = sinhf(x);
	} else {
		e = expf(x);
		ei = 0.5f / e;
		e = 0.5f * e;
		*s = e - ei;
		*c = e + ei;
	}
}

static double redupi(double x)
{
	double t;
	long i;

	t = x / M_PI;
	if (t >= 0.0)
		t += 0.5;
	else
		t -= 0.5;

	i = t;
	t = i;
	t = ((x - t * DP1) - t * DP2) - t * DP3;
	return t;
}

static float redupif(float x)
{
	float t;
	long i;

	t = x / (float)M_PI;
	if (t >= 0.0f)
		t += 0.5f;
	else
		t -= 0.5f;

	i = t;
	t = i;
	t = ((x - t * DP1) - t * DP2) - t * DP3;
	return t;
}

static long double redupil(long double x)
{
	long double t;
	long long i;

	t = x / M_PIL;
	if (t >= 0.0L)
		t += 0.5L;
	else
		t -= 0.5L;

	i = t;
	t = i;
	return ((x - t * DP1) - t * DP2) - t * DP3;
}

static long double ctansl(long double complex z)
{
	long double f, x, x2, y, y2, rn, t;
	long double d;

	x = fabsl(2.0L * creall(z));
	y = fabsl(2.0L * cimagl(z));

	x = redupil(x);

	x = x * x;
	y = y * y;
	x2 = 1.0L;
	y2 = 1.0L;
	f = 1.0L;
	rn = 0.0L;
	d = 0.0L;
	do {
		rn += 1.0L;
		f *= rn;
		rn += 1.0L;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 + x2;
		t /= f;
		d += t;

		rn += 1.0L;
		f *= rn;
		rn += 1.0L;
		f *= rn;
		x2 *= x;
		y2 *= y;
		t = y2 - x2;
		t /= f;
		d += t;
	} while (fabsl(t / d) > MACHEPL);

	return d;
}

#endif
