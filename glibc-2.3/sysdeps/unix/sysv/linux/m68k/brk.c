/* brk system call for Linux/m68k.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
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

#include <errno.h>
#include <unistd.h>
#include <sysdep.h>

void *__curbrk = 0;

/* Old braindamage in GCC's crtstuff.c requires this symbol in an attempt
   to work around different old braindamage in the old Linux/x86 ELF
   dynamic linker.  Sigh.  */
weak_alias(__curbrk, ___brk_addr)

int
__brk(void *addr)
{
	void *newbrk;

	{
		register long d0 __asm__("%d0");

		asm("move%.l %2, %%d1\n"
		    "trap #0"		/* Perform the system call.  */
		    : "=d"(d0)
		    : "0"(SYS_ify(brk)), "g"(addr)
		    : "%d1");
		newbrk = (void *) d0;
	}

	__curbrk = newbrk;

	if (newbrk < addr) {
		__set_errno(ENOMEM);
		return -1;
	}

	return 0;
}

weak_alias(__brk, brk)
