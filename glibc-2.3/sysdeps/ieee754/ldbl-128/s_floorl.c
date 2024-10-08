/* s_floorl.c -- long double version of s_floor.c.
 * Conversion to IEEE quad long double by Jakub Jelinek, jj@ultra.linux.cz.
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
	int64_t i0, i1, j0;
	u_int64_t i, j;
	GET_LDOUBLE_WORDS64(i0, i1, x);
	j0 = ((i0 >> 48) & 0x7fff) - 0x3fff;

	if (j0 < 48) {
		if (j0 < 0) { 	/* raise inexact if x != 0 */
			if (huge + x > 0.0) { /* return 0*sign(x) if |x|<1 */
				if (i0 >= 0) {
					i0 = i1 = 0;

				} else if (((i0 & 0x7fffffffffffffffLL) | i1) != 0) {
					i0 = 0xbfff000000000000ULL;
					i1 = 0;
				}
			}

		} else {
			i = (0x0000ffffffffffffULL) >> j0;

			if (((i0 & i) | i1) == 0) return x; /* x is integral */

			if (huge + x > 0.0) {	/* raise inexact flag */
				if (i0 < 0) i0 += (0x0001000000000000LL) >> j0;

				i0 &= (~i);
				i1 = 0;
			}
		}

	} else if (j0 > 111) {
		if (j0 == 0x4000) return x + x;	/* inf or NaN */
		else return x;		/* x is integral */

	} else {
		i = -1ULL >> (j0 - 48);

		if ((i1 & i) == 0) return x;	/* x is integral */

		if (huge + x > 0.0) { 		/* raise inexact flag */
			if (i0 < 0) {
				if (j0 == 48) i0 += 1;

				else {
					j = i1 + (1LL << (112 - j0));

					if (j < i1) i0 += 1 ; 	/* got a carry */

					i1 = j;
				}
			}

			i1 &= (~i);
		}
	}

	SET_LDOUBLE_WORDS64(x, i0, i1);
	return x;
}

weak_alias(__floorl, floorl)
