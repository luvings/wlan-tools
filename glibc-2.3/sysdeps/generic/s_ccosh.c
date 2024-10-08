/* Complex cosine hyperbole function for double.
   Copyright (C) 1997 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <complex.h>
#include <fenv.h>
#include <math.h>

#include "math_private.h"


__complex__ double __ccosh(__complex__ double x)
{
	__complex__ double retval;
	int rcls = fpclassify(__real__ x);
	int icls = fpclassify(__imag__ x);

	if (rcls >= FP_ZERO) {
		/* Real part is finite.  */
		if (icls >= FP_ZERO) {
			/* Imaginary part is finite.  */
			double sinh_val = __ieee754_sinh(__real__ x);
			double cosh_val = __ieee754_cosh(__real__ x);
			double sinix, cosix;

			__sincos(__imag__ x, &sinix, &cosix);

			__real__ retval = cosh_val * cosix;
			__imag__ retval = sinh_val * sinix;

		} else {
			__imag__ retval = __real__ x == 0.0 ? 0.0 : __nan("");
			__real__ retval = __nan("") + __nan("");

#ifdef FE_INVALID

			if (icls == FP_INFINITE)
				feraiseexcept(FE_INVALID);

#endif
		}

	} else if (rcls == FP_INFINITE) {
		/* Real part is infinite.  */
		if (icls == FP_ZERO) {
			/* Imaginary part is 0.0.  */
			__real__ retval = HUGE_VAL;
			__imag__ retval = __imag__ x * __copysign(1.0, __real__ x);

		} else if (icls > FP_ZERO) {
			/* Imaginary part is finite.  */
			double sinix, cosix;

			__sincos(__imag__ x, &sinix, &cosix);

			__real__ retval = __copysign(HUGE_VAL, cosix);
			__imag__ retval = (__copysign(HUGE_VAL, sinix)
			                   * __copysign(1.0, __real__ x));

		} else {
			/* The addition raises the invalid exception.  */
			__real__ retval = HUGE_VAL;
			__imag__ retval = __nan("") + __nan("");

#ifdef FE_INVALID

			if (icls == FP_INFINITE)
				feraiseexcept(FE_INVALID);

#endif
		}

	} else {
		__real__ retval = __nan("");
		__imag__ retval = __imag__ x == 0.0 ? __imag__ x : __nan("");
	}

	return retval;
}

weak_alias(__ccosh, ccosh)
#ifdef NO_LONG_DOUBLE
strong_alias(__ccosh, __ccoshl)
weak_alias(__ccosh, ccoshl)
#endif
