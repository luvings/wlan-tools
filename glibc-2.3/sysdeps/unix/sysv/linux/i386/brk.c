/* brk system call for Linux/i386.
   Copyright (C) 1995, 1996, 2000 Free Software Foundation, Inc.
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

#include <bp-checks.h>

/* This must be initialized data because commons can't have aliases.  */
void *__curbrk = 0;

/* Old braindamage in GCC's crtstuff.c requires this symbol in an attempt
   to work around different old braindamage in the old Linux ELF dynamic
   linker.  */
weak_alias(__curbrk, ___brk_addr)

int
__brk(void *addr)
{
	void *__unbounded newbrk, *__unbounded scratch;

	asm("movl %%ebx, %1\n"	/* Save %ebx in scratch register.  */
	    "movl %3, %%ebx\n"	/* Put ADDR in %ebx to be syscall arg.  */
	    "int $0x80 # %2\n"	/* Perform the system call.  */
	    "movl %1, %%ebx\n"	/* Restore %ebx from scratch register.  */
	    : "=a"(newbrk), "=r"(scratch)
	    : "0"(SYS_ify(brk)), "g"(__ptrvalue(addr)));

	__curbrk = newbrk;

	if (newbrk < addr) {
		__set_errno(ENOMEM);
		return -1;
	}

	return 0;
}

weak_alias(__brk, brk)
