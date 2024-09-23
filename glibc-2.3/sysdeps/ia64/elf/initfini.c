/* Special .init and .fini section support for ia64.
   Copyright (C) 2000, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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

/* This file is compiled into assembly code which is then munged by a sed
   script into two files: crti.s and crtn.s.

   * crti.s puts a function prologue at the beginning of the
   .init and .fini sections and defines global symbols for
   those addresses, so they can be called as functions.

   * crtn.s puts the corresponding function epilogues
   in the .init and .fini sections. */

__asm__("\n\
\n\
#include \"defs.h\"\n\
\n\
/*@HEADER_ENDS*/\n\
\n\
/*@_init_PROLOG_BEGINS*/\n\
	.section .init\n\
	.align 16\n\
	.global _init#\n\
	.proc _init#\n\
_init:\n\
	alloc r34 = ar.pfs, 0, 3, 0, 0\n\
	mov r32 = r12\n\
	mov r33 = b0\n\
	adds r12 = -16, r12\n\
	addl r14 = @ltoff(@fptr(__gmon_start__#)), gp\n\
	;;\n\
	ld8 r15 = [r14]\n\
	;;\n\
	cmp.eq p6, p7 = 0, r15\n\
	(p6) br.cond.dptk .L5\n\
\n\
/* we could use r35 to save gp, but we use the stack since that's what\n\
 * all the other init routines will do --davidm 00/04/05 */\n\
	st8 [r12] = gp, -16\n\
	br.call.sptk.many b0 = __gmon_start__# ;;\n\
	adds r12 = 16, r12\n\
	;;\n\
	ld8 gp = [r12]\n\
	;;\n\
.L5:\n\
	.align 16\n\
	.endp _init#\n\
\n\
/*@_init_PROLOG_ENDS*/\n\
\n\
/*@_init_EPILOG_BEGINS*/\n\
	.section .init\n\
	.regstk 0,2,0,0\n\
	mov r12 = r32\n\
	mov ar.pfs = r34\n\
	mov b0 = r33\n\
	br.ret.sptk.many b0\n\
	.endp _init#\n\
/*@_init_EPILOG_ENDS*/\n\
\n\
/*@_fini_PROLOG_BEGINS*/\n\
	.section .fini\n\
	.align 16\n\
	.global _fini#\n\
	.proc _fini#\n\
_fini:\n\
	alloc r34 = ar.pfs, 0, 3, 0, 0\n\
	mov r32 = r12\n\
	mov r33 = b0\n\
	adds r12 = -16, r12\n\
	;;\n\
	.align 16\n\
	.endp _fini#\n\
\n\
/*@_fini_PROLOG_ENDS*/\n\
	br.call.sptk.many b0 = i_am_not_a_leaf# ;;\n\
	;;\n\
\n\
/*@_fini_EPILOG_BEGINS*/\n\
	.section .fini\n\
	mov r12 = r32\n\
	mov ar.pfs = r34\n\
	mov b0 = r33\n\
	br.ret.sptk.many b0\n\
	.endp _fini#\n\
\n\
/*@_fini_EPILOG_ENDS*/\n\
\n\
/*@TRAILER_BEGINS*/\n\
	.weak	__gmon_start__#\n\
");
