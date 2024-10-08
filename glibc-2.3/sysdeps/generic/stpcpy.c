/* Copyright (C) 1992, 1995, 1997, 2002 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#undef __stpcpy
#undef stpcpy

#ifndef weak_alias
# define __stpcpy stpcpy
#endif

/* Copy SRC to DEST, returning the address of the terminating '\0' in DEST.  */
char *__stpcpy(dest, src)
char *dest;
const char *src;
{
	register char *d = dest;
	register const char *s = src;

	do
		*d++ = *s;

	while (*s++ != '\0');

	return d - 1;
}

#ifdef libc_hidden_def
libc_hidden_def(__stpcpy)
#endif
#ifdef weak_alias
weak_alias(__stpcpy, stpcpy)
#endif
