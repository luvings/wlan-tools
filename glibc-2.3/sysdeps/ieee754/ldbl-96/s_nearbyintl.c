/* s_rintl.c -- long double version of s_rint.c.
 * Conversion to long double by Ulrich Drepper,
 * Cygnus Support, drepper@cygnus.com.
 */
/* Adapted for use as nearbyint by Ulrich Drepper <drepper@cygnus.com>.  */

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

/*
 * rintl(x)
 * Return x rounded to integral value according to the prevailing
 * rounding mode.
 * Method:
 *	Using floating addition.
 * Exception:
 *	Inexact flag raised if x not equal to rintl(x).
 */

#include <fenv.h>
#include "math.h"
#include "math_private.h"

#ifdef __STDC__
static const long double
#else
static long double
#endif
TWO63[2] = {
	9.223372036854775808000000e+18, /* 0x403E, 0x00000000, 0x00000000 */
	-9.223372036854775808000000e+18  /* 0xC03E, 0x00000000, 0x00000000 */
};

#ifdef __STDC__
long double __nearbyintl(long double x)
#else
long double __nearbyintl(x)
long double x;
#endif
{
	fenv_t env;
	int32_t se, j0, sx;
	u_int32_t i, i0, i1;
	long double w, t;
	GET_LDOUBLE_WORDS(se, i0, i1, x);
	sx = (se >> 15) & 1;
	j0 = (se & 0x7fff) - 0x3fff;

	if (j0 < 31) {
		if (j0 < 0) {
			if (((se & 0x7fff) | i0 | i1) == 0) return x;

			i1 |= i0;
			i0 &= 0xe0000000;
			i0 |= (i1 | -i1) & 0x80000000;
			SET_LDOUBLE_MSW(x, i0);
			feholdexcept(&env);
			w = TWO63[sx] + x;
			t = w - TWO63[sx];
			fesetenv(&env);
			GET_LDOUBLE_EXP(i0, t);
			SET_LDOUBLE_EXP(t, (i0 & 0x7fff) | (sx << 15));
			return t;

		} else {
			i = (0x7fffffff) >> j0;

			if (((i0 & i) | i1) == 0) return x; /* x is integral */

			i >>= 1;

			if (((i0 & i) | i1) != 0) {
				if (j0 == 30) i1 = 0x40000000;

				else
					i0 = (i0 & (~i)) | ((0x20000000) >> j0);
			}
		}

	} else if (j0 > 62) {
		if (j0 == 0x4000) return x + x;	/* inf or NaN */
		else return x;		/* x is integral */

	} else {
		i = ((u_int32_t)(0xffffffff)) >> (j0 - 31);

		if ((i1 & i) == 0) return x;	/* x is integral */

		i >>= 1;

		if ((i1 & i) != 0) i1 = (i1 & (~i)) | ((0x40000000) >> (j0 - 31));
	}

	SET_LDOUBLE_WORDS(x, se, i0, i1);
	feholdexcept(&env);
	w = TWO63[sx] + x;
	t = w - TWO63[sx];
	fesetenv(&env);
	return t;
}

weak_alias(__nearbyintl, nearbyintl)
