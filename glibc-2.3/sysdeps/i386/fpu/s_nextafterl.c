/* s_nextafterl.c -- long double version of s_nextafter.c.
 * Special version for i387.
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

/* IEEE functions
 *	nextafterl(x,y)
 *	return the next machine floating-point number of x in the
 *	direction toward y.
 *   Special cases:
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
long double __nextafterl(long double x, long double y)
#else
long double __nextafterl(x, y)
long double x, y;
#endif
{
	u_int32_t hx, hy, ix, iy;
	u_int32_t lx, ly;
	int32_t esx, esy;

	GET_LDOUBLE_WORDS(esx, hx, lx, x);
	GET_LDOUBLE_WORDS(esy, hy, ly, y);
	ix = esx & 0x7fff;		/* |x| */
	iy = esy & 0x7fff;		/* |y| */

	/* Intel's extended format has the normally implicit 1 explicit
	   present.  Sigh!  */
	if (((ix == 0x7fff) && (((hx & 0x7fffffff) | lx) != 0)) || /* x is nan */
	                ((iy == 0x7fff) && (((hy & 0x7fffffff) | ly) != 0))) /* y is nan */
		return x + y;

	if (x == y) return y;		/* x=y, return y */

	if ((ix | hx | lx) == 0) {			/* x == 0 */
		SET_LDOUBLE_WORDS(x, esy & 0x8000, 0, 1); /* return +-minsubnormal */
		y = x * x;

		if (y == x) return y;

		else return x;	/* raise underflow flag */
	}

	if (esx >= 0) {			/* x > 0 */
		if (esx > esy || ((esx == esy) && (hx > hy || ((hx == hy) && (lx > ly))))) {
			/* x > y, x -= ulp */
			if (lx == 0) {
				if (hx <= 0x80000000) {
					if (esx == 0)
						hx = 0;

					else {
						esx -= 1;
						hx = hx - 1;

						if (esx > 0)
							hx |= 0x80000000;
					}

				} else
					hx -= 1;
			}

			lx -= 1;

		} else {				/* x < y, x += ulp */
			lx += 1;

			if (lx == 0) {
				hx += 1;

				if (hx == 0 || (esx == 0 && hx == 0x80000000)) {
					esx += 1;
					hx |= 0x80000000;
				}
			}
		}

	} else {				/* x < 0 */
		if (esy >= 0 || (esx > esy || ((esx == esy) && (hx > hy || ((hx == hy) && (lx > ly)))))) {
			/* x < y, x -= ulp */
			if (lx == 0) {
				if (hx <= 0x80000000) {
					esx -= 1;
					hx = hx - 1;

					if ((esx & 0x7fff) > 0)
						hx |= 0x80000000;

				} else
					hx -= 1;
			}

			lx -= 1;

		} else {				/* x > y, x += ulp */
			lx += 1;

			if (lx == 0) {
				hx += 1;

				if (hx == 0 || (esx == 0xffff8000 && hx == 0x80000000)) {
					esx += 1;
					hx |= 0x80000000;
				}
			}
		}
	}

	esy = esx & 0x7fff;

	if (esy == 0x7fff) return x + x;	/* overflow  */

	if (esy == 0) {			/* underflow */
		y = x * x;

		if (y != x) {		/* raise underflow flag */
			SET_LDOUBLE_WORDS(y, esx, hx, lx);
			return y;
		}
	}

	SET_LDOUBLE_WORDS(x, esx, hx, lx);
	return x;
}

weak_alias(__nextafterl, nextafterl)
strong_alias(__nextafterl, __nexttowardl)
weak_alias(__nextafterl, nexttowardl)
