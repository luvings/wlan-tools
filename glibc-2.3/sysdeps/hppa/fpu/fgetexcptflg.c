/* Store current representation for exceptions.
   Copyright (C) 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by David Huggins-Daines <dhd@debian.org>, 2000

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

#include <fenv.h>

int fegetexceptflag(fexcept_t *flagp, int excepts)
{
	unsigned int sw[2];

	/* Get the current status word. */
	__asm__("fstd %%fr0,0(%1)" : "=m"(*sw) : "r"(sw));

	*flagp = (sw[0] >> 27) & excepts & FE_ALL_EXCEPT;

	/* Success.  */
	return 0;
}

