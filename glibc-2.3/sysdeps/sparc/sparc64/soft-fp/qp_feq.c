/* Software floating-point emulation.
   Return 1 if (*a) == (*b)
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
#include "quad.h"

int _Qp_feq(const long double *a, const long double *b)
{
	FP_DECL_EX;
	FP_DECL_Q(A);
	FP_DECL_Q(B);
	int r;

	FP_INIT_ROUNDMODE;
	FP_UNPACK_RAW_QP(A, a);
	FP_UNPACK_RAW_QP(B, b);
	FP_CMP_EQ_Q(r, A, B);

	if (r && (FP_ISSIGNAN_Q(A) || FP_ISSIGNAN_Q(B)))
		FP_SET_EXCEPTION(FP_EX_INVALID);

	QP_HANDLE_EXCEPTIONS(
	        __asm(
	                "	ldd [%0], %%f52\n"
	                "	ldd [%0+8], %%f54\n"
	                "	ldd [%1], %%f56\n"
	                "	ldd [%1+8], %%f58\n"
	                "	fcmpq %%fcc3, %%f52, %%f56\n"
	                "	" : : "r"(a), "r"(b) : QP_CLOBBER_CC);
	        _FPU_GETCW(_fcw);
	        r = ((_fcw >> 36) & 3));

	return !r;
}
