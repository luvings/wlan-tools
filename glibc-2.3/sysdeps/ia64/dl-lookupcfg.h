/* Configuration of lookup functions.
   Copyright (C) 2000, 2001 Free Software Foundation, Inc.
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

/* The ia64 need more information from the symbol lookup function
   than just the address. */
#define DL_LOOKUP_RETURNS_MAP
#define ELF_FUNCTION_PTR_IS_SPECIAL
#define DL_UNMAP_IS_SPECIAL

/* Forward declaration.  */
struct link_map;

extern void *_dl_symbol_address(const struct link_map *map,
                                const Elf64_Sym *ref);

#define DL_SYMBOL_ADDRESS(map, ref) _dl_symbol_address(map, ref)

extern Elf64_Addr _dl_lookup_address(const void *address);

#define DL_LOOKUP_ADDRESS(addr) _dl_lookup_address (addr)

extern void _dl_unmap(struct link_map *map);

#define DL_UNMAP(map) _dl_unmap (map)

#define DL_AUTO_FUNCTION_ADDRESS(map, addr)		\
({							\
  unsigned long int fptr[2];				\
  fptr[0] = (unsigned long int) (addr);			\
  fptr[1] = (map)->l_info[DT_PLTGOT]->d_un.d_ptr;	\
  (Elf64_Addr) fptr;					\
})

#define DL_STATIC_FUNCTION_ADDRESS(map, addr)		\
({							\
  static unsigned long int fptr[2];			\
  fptr[0] = (unsigned long int) (addr);			\
  fptr[1] = (map)->l_info[DT_PLTGOT]->d_un.d_ptr;	\
  (Elf64_Addr) fptr;					\
})

#define DL_DT_INIT_ADDRESS(map, addr) DL_AUTO_FUNCTION_ADDRESS (map, addr)
#define DL_DT_FINI_ADDRESS(map, addr) DL_AUTO_FUNCTION_ADDRESS (map, addr)
