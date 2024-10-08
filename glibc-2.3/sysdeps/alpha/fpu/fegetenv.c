/* Store current floating-point environment.
   Copyright (C) 1997,99,2000,01 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson <rth@tamu.edu>, 1997

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

int __fegetenv(fenv_t *envp)
{
	unsigned long int fpcr;
	unsigned long int swcr;

	/* Get status from software and hardware.  Note that we don't need an
	   excb because the callsys is an implied trap barrier.  */
	swcr = __ieee_get_fp_control();
	__asm__ __volatile__("mf_fpcr %0" : "=f"(fpcr));

	/* Merge the two bits of information.  */
	*envp = ((fpcr & FPCR_ROUND_MASK) | (swcr & SWCR_ALL_MASK));

	/* Success.  */
	return 0;
}

#include <shlib-compat.h>
#if SHLIB_COMPAT (libm, GLIBC_2_1, GLIBC_2_2)
strong_alias(__fegetenv, __old_fegetenv)
compat_symbol(libm, __old_fegetenv, fegetenv, GLIBC_2_1);
#endif

versioned_symbol(libm, __fegetenv, fegetenv, GLIBC_2_2);
