/* Raise given exceptions.
   Copyright (C) 1997,98,99,2000,01,02 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson <rth@tamu.edu>, 1997.

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

#include <fenv_libc.h>

int __feraiseexcept(int excepts)
{
	unsigned long int tmp;

	/* Get the current exception state.  */
	tmp = __ieee_get_fp_control();

	/* Set all the bits that were called for.  */
	tmp |= (excepts & SWCR_STATUS_MASK);

	/* And store it back.  */
	__ieee_set_fp_control(tmp);

	/* Success.  */
	return 0;
}

#include <shlib-compat.h>
#if SHLIB_COMPAT (libm, GLIBC_2_1, GLIBC_2_2)
strong_alias(__feraiseexcept, __old_feraiseexcept)
compat_symbol(libm, __old_feraiseexcept, feraiseexcept, GLIBC_2_1);
#endif

libm_hidden_ver(__feraiseexcept, feraiseexcept)
versioned_symbol(libm, __feraiseexcept, feraiseexcept, GLIBC_2_2);
