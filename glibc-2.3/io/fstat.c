/* Copyright (C) 1996, 1997, 1998, 2001 Free Software Foundation, Inc.
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

#include <sys/stat.h>

/* This definition is only used if inlining fails for this function; see
   the last page of <sys/stat.h>.  The real work is done by the `x'
   function which is passed a version number argument.  We arrange in the
   makefile that when not inlined this function is always statically
   linked; that way a dynamically-linked executable always encodes the
   version number corresponding to the data structures it uses, so the `x'
   functions in the shared library can adapt without needing to recompile
   all callers.  */

#undef fstat
#undef __fstat
int __fstat(int fd, struct stat *buf)
{
	return __fxstat(_STAT_VER, fd, buf);
}

weak_alias(__fstat, fstat)

/* Hide the symbol so that no definition but the one locally in the
   executable or DSO is used.  */
#ifdef HAVE_DOT_HIDDEN
asm(".hidden\tfstat");
asm(".hidden\t__fstat");
#endif
