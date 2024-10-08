/* Special .init and .fini section support for HPPA
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

/* If we use the standard C version, the linkage table pointer won't
   be properly preserved due to the splitting up of function prologues
   and epilogues.  Therefore we write these in assembly to make sure
   they do the right thing.

   Note that we cannot have a weak undefined __gmon_start__, because
   that would require this to be PIC, and the linker is currently not
   able to generate a proper procedure descriptor for _init.  Sad but
   true.  Anyway, HPPA is one of those horrible architectures where
   making the comparison and indirect call is quite expensive (see the
   comment in sysdeps/generic/initfini.c). */

__asm__("\
\n\
#include \"defs.h\"\n\
\n\
/*@HEADER_ENDS*/\n\
\n\
/*@_init_PROLOG_BEGINS*/\n\
	.section .init\n\
	.align 4\n\
	.globl _init\n\
	.type _init,@function\n\
_init:\n\
	stw	%rp,-20(%sp)\n\
	stwm	%r4,64(%sp)\n\
	stw	%r19,-32(%sp)\n\
	bl	__gmon_start__,%rp\n\
	copy	%r19,%r4	/* delay slot */\n\
	copy	%r4,%r19\n\
/*@_init_PROLOG_ENDS*/\n\
\n\
/*@_init_EPILOG_BEGINS*/\n\
        .text\n\
        .align 4\n\
        .weak   __gmon_start__\n\
        .type    __gmon_start__,@function\n\
__gmon_start__:\n\
	.proc\n\
	.callinfo\n\
	.entry\n\
        bv,n %r0(%r2)\n\
	.exit\n\
	.procend\n\
\n\
/* Here is the tail end of _init.  We put __gmon_start before this so\n\
   that the assembler creates the .PARISC.unwind section for us, ie.\n\
   with the right attributes.  */\n\
	.section .init\n\
	ldw	-84(%sp),%rp\n\
	copy	%r4,%r19\n\
	bv	%r0(%rp)\n\
_end_init:\n\
	ldwm	-64(%sp),%r4\n\
\n\
/* Our very own unwind info, because the assembler can't handle\n\
   functions split into two or more pieces.  */\n\
	.section .PARISC.unwind\n\
	.extern _init\n\
	.word	_init, _end_init\n\
	.byte	0x08, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08\n\
\n\
/*@_init_EPILOG_ENDS*/\n\
\n\
/*@_fini_PROLOG_BEGINS*/\n\
	.section .fini\n\
	.align 4\n\
	.globl _fini\n\
	.type _fini,@function\n\
_fini:\n\
	stw	%rp,-20(%sp)\n\
	stwm	%r4,64(%sp)\n\
	stw	%r19,-32(%sp)\n\
	copy	%r19,%r4\n\
/*@_fini_PROLOG_ENDS*/\n\
\n\
/*@_fini_EPILOG_BEGINS*/\n\
	.section .fini\n\
	ldw	-84(%sp),%rp\n\
	copy	%r4,%r19\n\
	bv	%r0(%rp)\n\
_end_fini:\n\
	ldwm	-64(%sp),%r4\n\
\n\
	.section .PARISC.unwind\n\
	.extern _fini\n\
	.word	_fini, _end_fini\n\
	.byte	0x08, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08\n\
\n\
/*@_fini_EPILOG_ENDS*/\n\
\n\
/*@TRAILER_BEGINS*/\
");
