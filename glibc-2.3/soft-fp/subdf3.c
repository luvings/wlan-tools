/* Software floating-point emulation.
   Return a - b
   Copyright (C) 1997,1999 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson (rth@cygnus.com) and
		  Jakub Jelinek (jj@ultra.linux.cz).

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

#include "soft-fp.h"
#include "double.h"

double __subdf3(double a, double b)
{
	FP_DECL_EX;
	FP_DECL_D(A);
	FP_DECL_D(B);
	FP_DECL_D(R);
	double r;

	FP_INIT_ROUNDMODE;
	FP_UNPACK_D(A, a);
	FP_UNPACK_D(B, b);
	FP_SUB_D(R, A, B);
	FP_PACK_D(r, R);
	FP_HANDLE_EXCEPTIONS;

	return r;
}
