/* Special .init and .fini section support for S/390.
   Copyright (C) 2000, 2001, 2002 Free Software Foundation, Inc.
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

__asm__("\
\n\
#include \"defs.h\"\n\
\n\
/*@HEADER_ENDS*/\n\
\n\
/*@TESTS_BEGIN*/\n\
\n\
/*@TESTS_END*/\n\
\n\
/*@_init_PROLOG_BEGINS*/\n\
\n\
	.section .init\n\
#NO_APP\n\
	.align 4\n\
.globl _init\n\
	.type	 _init,@function\n\
_init:\n\
#	leaf function           0\n\
#	automatics              0\n\
#	outgoing args           0\n\
#	need frame pointer      0\n\
#	call alloca             0\n\
#	has varargs             0\n\
#	incoming args (stack)   0\n\
#	function length         36\n\
	STM	6,15,24(15)\n\
	BRAS	13,.LTN1_0\n\
.LT1_0:\n\
.LC14:\n\
	.long	__gmon_start__@GOT\n\
.LC15:\n\
	.long	_GLOBAL_OFFSET_TABLE_-.LT1_0\n\
.LTN1_0:\n\
	LR	1,15\n\
	AHI	15,-96\n\
	ST	1,0(15)\n\
	L	12,.LC15-.LT1_0(13)\n\
	AR	12,13\n\
	L     1,.LC14-.LT1_0(13)\n\
	L     1,0(1,12)\n\
	LTR   1,1\n\
	JE    .L22\n\
	BASR  14,1\n\
.L22:\n\
#APP\n\
	.align 4,0x07\n\
	END_INIT\n\
\n\
/*@_init_PROLOG_ENDS*/\n\
\n\
/*@_init_EPILOG_BEGINS*/\n\
	.align 4\n\
	.section .init\n\
#NO_APP\n\
	.align 4\n\
	L	4,152(15)\n\
	LM	6,15,120(15)\n\
	BR	4\n\
#APP\n\
	END_INIT\n\
\n\
/*@_init_EPILOG_ENDS*/\n\
\n\
/*@_fini_PROLOG_BEGINS*/\n\
	.section .fini\n\
#NO_APP\n\
	.align 4\n\
.globl _fini\n\
	.type	 _fini,@function\n\
_fini:\n\
#	leaf function           0\n\
#	automatics              0\n\
#	outgoing args           0\n\
#	need frame pointer      0\n\
#	call alloca             0\n\
#	has varargs             0\n\
#	incoming args (stack)   0\n\
#	function length         30\n\
	STM	6,15,24(15)\n\
	BRAS	13,.LTN2_0\n\
.LT2_0:\n\
.LC17:\n\
	.long	_GLOBAL_OFFSET_TABLE_-.LT2_0\n\
.LTN2_0:\n\
	LR	1,15\n\
	AHI	15,-96\n\
	ST	1,0(15)\n\
	L	12,.LC17-.LT2_0(13)\n\
	AR	12,13\n\
#APP\n\
	.align 4,0x07\n\
	END_FINI\n\
\n\
/*@_fini_PROLOG_ENDS*/\n\
\n\
/*@_fini_EPILOG_BEGINS*/\n\
	.align 4\n\
	.section .fini\n\
#NO_APP\n\
	.align 4\n\
	L	4,152(15)\n\
	LM	6,15,120(15)\n\
	BR	4\n\
#APP\n\
	END_FINI\n\
\n\
/*@_fini_EPILOG_ENDS*/\n\
\n\
/*@TRAILER_BEGINS*/\
");
