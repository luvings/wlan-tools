/* Get the symbol address.  IA-64 version.
   Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
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

#include <ldsodefs.h>
#include <dl-machine.h>

void *_dl_symbol_address(const struct link_map *map, const Elf64_Sym *ref)
{
	Elf64_Addr value = (map ? map->l_addr : 0) + ref->st_value;

	/* On ia64, we have to return the pointer to function descriptor. */
	if (ELFW(ST_TYPE)(ref->st_info) == STT_FUNC)
		return (void *) __ia64_make_fptr(map, ref, value);

	else
		return (void *) value;
}
