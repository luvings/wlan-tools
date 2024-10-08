/* Copyright (C) 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2001.

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

/* For iconv we don't have to handle repertoire maps.  Provide dummy
   definitions to allow the use of linereader.c unchanged.  */
#include <repertoire.h>


uint32_t repertoire_find_value(const struct repertoire_t *repertoire, const char *name,
                               size_t len)
{
	return ILLEGAL_CHAR_VALUE;
}


const char *repertoire_find_symbol(const struct repertoire_t *repertoire, uint32_t ucs)
{
	return NULL;
}
