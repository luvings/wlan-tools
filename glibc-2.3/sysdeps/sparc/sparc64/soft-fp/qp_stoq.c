/* Software floating-point emulation.
   (*c) = (long double)(a)
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
#include "single.h"
#include "quad.h"

void _Qp_stoq(long double *c, const float a)
{
	FP_DECL_EX;
	FP_DECL_S(A);
	FP_DECL_Q(C);

	FP_INIT_ROUNDMODE;
	FP_UNPACK_S(A, a);
#if (2 * _FP_W_TYPE_SIZE) < _FP_FRACBITS_Q
	FP_CONV(Q, S, 4, 1, C, A);
#else
	FP_CONV(Q, S, 2, 1, C, A);
#endif
	FP_PACK_QP(c, C);
	QP_HANDLE_EXCEPTIONS(__asm(
	                             "	fstoq %1, %%f60\n"
	                             "	std %%f60, [%0]\n"
	                             "	std %%f62, [%0+8]\n"
	                             "	" : : "r"(c), "f"(a) : QP_CLOBBER));
}
