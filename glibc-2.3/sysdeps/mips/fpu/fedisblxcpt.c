/* Disable floating-point exceptions.
   Copyright (C) 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Jaeger <aj@suse.de>, 2000.

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
#include <fenv_libc.h>
#include <fpu_control.h>

int fedisableexcept(int excepts)
{
	unsigned int new_exc, old_exc;

	/* Get the current control word.  */
	_FPU_GETCW(new_exc);

	old_exc = (new_exc & ENABLE_MASK) >> ENABLE_SHIFT;

	excepts &= FE_ALL_EXCEPT;

	new_exc &= ~(excepts << ENABLE_SHIFT);
	new_exc &= ~_FPU_RESERVED;
	_FPU_SETCW(new_exc);

	return old_exc;
}
