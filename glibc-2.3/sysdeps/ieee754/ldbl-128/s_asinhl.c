/* s_asinhl.c -- long double version of s_asinh.c.
 * Conversion to long double by Ulrich Drepper,
 * Cygnus Support, drepper@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$NetBSD: $";
#endif

/* asinhl(x)
 * Method :
 *      Based on
 *              asinhl(x) = signl(x) * logl [ |x| + sqrtl(x*x+1) ]
 *      we have
 *      asinhl(x) := x  if  1+x*x=1,
 *                := signl(x)*(logl(x)+ln2)) for large |x|, else
 *                := signl(x)*logl(2|x|+1/(|x|+sqrtl(x*x+1))) if|x|>2, else
 *                := signl(x)*log1pl(|x| + x^2/(1 + sqrtl(1+x^2)))
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
static const long double
#else
static long double
#endif
one = 1.0L,
ln2 = 6.931471805599453094172321214581765681e-1L,
huge = 1.0e+4900L;

#ifdef __STDC__
long double
__asinhl(long double x)
#else
long double
__asinhl(x)
long double x;
#endif
{
	long double t, w;
	int32_t ix, sign;
	ieee854_long_double_shape_type u;

	u.value = x;
	sign = u.parts32.w0;
	ix = sign & 0x7fffffff;

	if (ix == 0x7fff0000)
		return x + x;		/* x is inf or NaN */

	if (ix < 0x3fc70000) {
		/* |x| < 2^ -56 */
		if (huge + x > one)
			return x;		/* return x inexact except 0 */
	}

	u.parts32.w0 = ix;

	if (ix > 0x40350000) {
		/* |x| > 2 ^ 54 */
		w = __ieee754_logl(u.value) + ln2;

	} else if (ix > 0x40000000) {
		/* 2^ 54 > |x| > 2.0 */
		t = u.value;
		w = __ieee754_logl(2.0 * t + one / (__ieee754_sqrtl(x * x + one) + t));

	} else {
		/* 2.0 > |x| > 2 ^ -56 */
		t = x * x;
		w = __log1pl(u.value + t / (one + __ieee754_sqrtl(one + t)));
	}

	if (sign & 0x80000000)
		return -w;

	else
		return w;
}

weak_alias(__asinhl, asinhl)
