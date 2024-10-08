/* Copyright (C) 1999, 2000, 2002 Free Software Foundation, Inc.
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

/* Ugly trick ahead to make the alias work.  */
#define imaxdiv __libc_imaxdiv

#include <inttypes.h>

#ifdef SHARED
/* This is an ugly trick.  We cause the C code generated for the code
   in lldiv.c to use __divdi3_internal instead of __divdi3 by defining
   an alias on the assembler level.  */
asm("__divdi3 = __divdi3_internal");
#endif

#include <sysdeps/generic/lldiv.c>

#undef imaxdiv
weak_alias(lldiv, imaxdiv)
