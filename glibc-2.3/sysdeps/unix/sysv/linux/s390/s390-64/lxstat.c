/* lxstat using old-style Unix fstat system call.  64 bit S/390 version.
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
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

/* Ho hum, since xstat == xstat64 we must get rid of the prototype or gcc
   will complain since they don't strictly match.  */
#define __lxstat64 __lxstat64_disable

#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

#include <sysdep.h>
#include <sys/syscall.h>

extern int __syscall_lstat(const char *, struct stat *);

/* Get information about the file FD in BUF.  */
int __lxstat(int vers, const char *name, struct stat *buf)
{
	return INLINE_SYSCALL(lstat, 2, name, buf);
}

hidden_def(__lxstat)
weak_alias(__lxstat, _lxstat);
#undef __lxstat64
strong_alias(__lxstat, __lxstat64);
hidden_ver(__lxstat, __lxstat64)
