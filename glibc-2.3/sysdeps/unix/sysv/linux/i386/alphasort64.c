/* Copyright (C) 1992, 1997, 1998, 2000 Free Software Foundation, Inc.
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

#include <dirent.h>
#include <string.h>

int __alphasort64(const void *a, const void *b)
{
	return strcoll((*(const struct dirent64 **) a)->d_name,
	               (*(const struct dirent64 **) b)->d_name);
}

#include <shlib-compat.h>

versioned_symbol(libc, __alphasort64, alphasort64, GLIBC_2_2);

#if SHLIB_COMPAT(libc, GLIBC_2_1, GLIBC_2_2)

#include <sysdeps/unix/sysv/linux/i386/olddirent.h>

int __old_alphasort64(const void *a, const void *b);

int __old_alphasort64(const void *a, const void *b)
{
	return strcoll((*(const struct __old_dirent64 **) a)->d_name,
	               (*(const struct __old_dirent64 **) b)->d_name);
}

compat_symbol(libc, __old_alphasort64, alphasort64, GLIBC_2_1);
#endif
