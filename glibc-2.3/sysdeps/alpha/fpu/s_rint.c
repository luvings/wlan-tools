/* Copyright (C) 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson.

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

#include <math.h>


double __rint(double x)
{
	if (isless(fabs(x), 9007199254740992.0)) {	/* 1 << DBL_MANT_DIG */
		double tmp1, new_x;
		__asm(
#ifdef _IEEE_FP_INEXACT
		        "cvttq/svid %2,%1\n\t"
#else
		        "cvttq/svd %2,%1\n\t"
#endif
		        "cvtqt/d %1,%0\n\t"
		        : "=f"(new_x), "=&f"(tmp1)
		        : "f"(x));

		/* rint(-0.1) == -0, and in general we'll always have the same
		 sign as our input.  */
		x = copysign(new_x, x);
	}

	return x;
}

weak_alias(__rint, rint)
#ifdef NO_LONG_DOUBLE
strong_alias(__rint, __rintl)
weak_alias(__rint, rintl)
#endif
