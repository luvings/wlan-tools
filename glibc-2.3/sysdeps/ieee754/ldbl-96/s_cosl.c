/* s_cosl.c -- long double version of s_cos.c.
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

/* cosl(x)
 * Return cosine function of x.
 *
 * kernel function:
 *	__kernel_sinl		... sine function on [-pi/4,pi/4]
 *	__kernel_cosl		... cosine function on [-pi/4,pi/4]
 *	__ieee754_rem_pio2l	... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *	[-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *	in [-pi/4 , +pi/4], and let n = k mod 4.
 *	We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *	    0	       S	   C		 T
 *	    1	       C	  -S		-1/T
 *	    2	      -S	  -C		 T
 *	    3	      -C	   S		-1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *	TRIG(x) returns trig(x) nearly rounded
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
long double __cosl(long double x)
#else
long double __cosl(x)
long double x;
#endif
{
	long double y[2], z = 0.0;
	int32_t n, se, i0, i1;

	/* High word of x. */
	GET_LDOUBLE_WORDS(se, i0, i1, x);

	/* |x| ~< pi/4 */
	se &= 0x7fff;

	if (se < 0x3ffe || (se == 0x3ffe && i0 <= 0xc90fdaa2))
		return __kernel_cosl(x, z);

	/* cos(Inf or NaN) is NaN */
	else if (se == 0x7fff) return x - x;

	/* argument reduction needed */
	else {
		n = __ieee754_rem_pio2l(x, y);

		switch (n & 3) {
		case 0:
			return  __kernel_cosl(y[0], y[1]);

		case 1:
			return -__kernel_sinl(y[0], y[1], 1);

		case 2:
			return -__kernel_cosl(y[0], y[1]);

		default:
			return  __kernel_sinl(y[0], y[1], 1);
		}
	}
}

weak_alias(__cosl, cosl)
