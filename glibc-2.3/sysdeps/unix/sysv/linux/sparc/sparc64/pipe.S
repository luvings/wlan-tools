/* Copyright (C) 1997, 1999, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Miguel de Icaza <miguel@gnu.ai.mit.edu>, 1997.

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

#include <sysdep.h>

ENTRY (__libc_pipe)
	mov	%o0, %o2		/* Save PIPEDES. */
	LOADSYSCALL(pipe)
	ta	0x6d
	bcc,pt	%xcc, 2f
	 nop
	SYSCALL_ERROR_HANDLER

2:	st	%o0, [%o2]		/* PIPEDES[0] = %o0; */
	st	%o1, [%o2 + 4]		/* PIPEDES[1] = %o1; */
	retl
	 clr	%o0
PSEUDO_END (__libc_pipe)

weak_alias (__libc_pipe, __pipe)
libc_hidden_def (__pipe)
weak_alias (__libc_pipe, pipe)
