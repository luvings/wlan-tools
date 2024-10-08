/* Software floating-point emulation.
   Return 0 iff a == b, 1 iff a > b, -2 iff a ? b, -1 iff a < b
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

int __gedf2(double a, double b)
{
	FP_DECL_EX;
	FP_DECL_D(A);
	FP_DECL_D(B);
	int r;

	FP_UNPACK_RAW_D(A, a);
	FP_UNPACK_RAW_D(B, b);
	FP_CMP_D(r, A, B, -2);

	if (r == -2 && (FP_ISSIGNAN_D(A) || FP_ISSIGNAN_D(B)))
		FP_SET_EXCEPTION(FP_EX_INVALID);

	FP_HANDLE_EXCEPTIONS;

	return r;
}

strong_alias(__gedf2, __gtdf2);
