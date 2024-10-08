/* s_floorl.c -- long double version of s_floor.c.
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

/*
 * floorl(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to floor(x).
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
static const long double huge = 1.0e4930;
#else
static long double huge = 1.0e4930;
#endif

#ifdef __STDC__
long double __floorl(long double x)
#else
long double __floorl(x)
long double x;
#endif
{
	int32_t i1, j0;
	u_int32_t i, j, se, i0, sx;
	GET_LDOUBLE_WORDS(se, i0, i1, x);
	sx = (se >> 15) & 1;
	j0 = (se & 0x7fff) - 0x3fff;

	if (j0 < 31) {
		if (j0 < 0) { 	/* raise inexact if x != 0 */
			if (huge + x > 0.0) { /* return 0*sign(x) if |x|<1 */
				if (sx == 0) {
					se = 0;
					i0 = i1 = 0;

				} else if (((se & 0x7fff) | i0 | i1) != 0) {
					se = 0xbfff;
					i0 = i1 = 0;
				}
			}

		} else {
			i = (0x7fffffff) >> j0;

			if (((i0 & i) | i1) == 0) return x; /* x is integral */

			if (huge + x > 0.0) {	/* raise inexact flag */
				if (sx) {
					if (j0 > 0 && (i0 + (0x80000000 >> j0)) > i0)
						i0 += (0x80000000) >> j0;

					else {
						i = 0x7fffffff;
						++se;
					}
				}

				i0 &= (~i);
				i1 = 0;
			}
		}

	} else if (j0 > 62) {
		if (j0 == 0x4000) return x + x;	/* inf or NaN */
		else return x;		/* x is integral */

	} else {
		i = ((u_int32_t)(0xffffffff)) >> (j0 - 31);

		if ((i1 & i) == 0) return x;	/* x is integral */

		if (huge + x > 0.0) { 		/* raise inexact flag */
			if (sx) {
				if (j0 == 31) i0 += 1;

				else {
					j = i1 + (1 << (63 - j0));

					if (j < i1) i0 += 1 ; 	/* got a carry */

					i1 = j;
				}
			}

			i1 &= (~i);
		}
	}

	SET_LDOUBLE_WORDS(x, se, i0, i1);
	return x;
}

weak_alias(__floorl, floorl)
