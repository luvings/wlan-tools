/* Copyright (C) 1995, 1996, 2001 Free Software Foundation, Inc.
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


#include <sys/types.h>
#include <sys/stat.h>

/* This definition is only used if inlining fails for this function; see
   the last page of <sys/stat.h>.  The real work is done by the `x'
   function which is passed a version number argument.  We arrange in the
   makefile that when not inlined this function is always statically
   linked; that way a dynamically-linked executable always encodes the
   version number corresponding to the data structures it uses, so the `x'
   functions in the shared library can adapt without needing to recompile
   all callers.  */

int __mknod(const char *path, mode_t mode, dev_t dev)
{
	return __xmknod(_MKNOD_VER, path, mode, &dev);
}

weak_alias(__mknod, mknod)

/* Hide the symbol so that no definition but the one locally in the
   executable or DSO is used.  */
#ifdef HAVE_DOT_HIDDEN
asm(".hidden\tmknod");
asm(".hidden\t__mknod");
#endif
