/* Enable floating-point exceptions.
   Copyright (C) 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2000.

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

int feenableexcept(int excepts)
{
	unsigned long int new_exc, old_exc;

	new_exc = __ieee_get_fp_control();

	old_exc = (new_exc & SWCR_ENABLE_MASK) << SWCR_ENABLE_SHIFT;
	new_exc |= (excepts >> SWCR_ENABLE_SHIFT) & SWCR_ENABLE_MASK;

	__ieee_set_fp_control(new_exc);

	return old_exc;
}
