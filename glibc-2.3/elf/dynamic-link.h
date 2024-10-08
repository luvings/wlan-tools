/* Inline functions for dynamic linking.
   Copyright (C) 1995,96,97,98,99,2000,2001,2002 Free Software Foundation, Inc.
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

#include <elf.h>
#include <dl-machine.h>
#include <assert.h>

#ifndef VERSYMIDX
# define VERSYMIDX(sym)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX (sym))
#endif


/* Read the dynamic section at DYN and fill in INFO with indices DT_*.  */

static inline void __attribute__((unused, always_inline))
elf_get_dynamic_info(struct link_map *l)
{
	ElfW(Dyn) *dyn = l->l_ld;
	ElfW(Dyn) **info;

#ifndef RTLD_BOOTSTRAP

	if (dyn == NULL)
		return;

#endif

	info = l->l_info;

	while (dyn->d_tag != DT_NULL) {
		if (dyn->d_tag < DT_NUM)
			info[dyn->d_tag] = dyn;

		else if (dyn->d_tag >= DT_LOPROC &&
		                dyn->d_tag < DT_LOPROC + DT_THISPROCNUM)
			info[dyn->d_tag - DT_LOPROC + DT_NUM] = dyn;

		else if ((Elf32_Word) DT_VERSIONTAGIDX(dyn->d_tag) < DT_VERSIONTAGNUM)
			info[VERSYMIDX(dyn->d_tag)] = dyn;

		else if ((Elf32_Word) DT_EXTRATAGIDX(dyn->d_tag) < DT_EXTRANUM)
			info[DT_EXTRATAGIDX(dyn->d_tag) + DT_NUM + DT_THISPROCNUM
			     + DT_VERSIONTAGNUM] = dyn;

		else if ((Elf32_Word) DT_VALTAGIDX(dyn->d_tag) < DT_VALNUM)
			info[DT_VALTAGIDX(dyn->d_tag) + DT_NUM + DT_THISPROCNUM
			     + DT_VERSIONTAGNUM + DT_EXTRANUM] = dyn;

		else if ((Elf32_Word) DT_ADDRTAGIDX(dyn->d_tag) < DT_ADDRNUM)
			info[DT_ADDRTAGIDX(dyn->d_tag) + DT_NUM + DT_THISPROCNUM
			     + DT_VERSIONTAGNUM + DT_EXTRANUM + DT_VALNUM] = dyn;

		++dyn;
	}

#ifndef DL_RO_DYN_SECTION

	/* Don't adjust .dynamic unnecessarily.  */
	if (l->l_addr != 0) {
		ElfW(Addr) l_addr = l->l_addr;

		if (info[DT_HASH] != NULL)
			info[DT_HASH]->d_un.d_ptr += l_addr;

		if (info[DT_PLTGOT] != NULL)
			info[DT_PLTGOT]->d_un.d_ptr += l_addr;

		if (info[DT_STRTAB] != NULL)
			info[DT_STRTAB]->d_un.d_ptr += l_addr;

		if (info[DT_SYMTAB] != NULL)
			info[DT_SYMTAB]->d_un.d_ptr += l_addr;

# if ! ELF_MACHINE_NO_RELA

		if (info[DT_RELA] != NULL)
			info[DT_RELA]->d_un.d_ptr += l_addr;

# endif
# if ! ELF_MACHINE_NO_REL

		if (info[DT_REL] != NULL)
			info[DT_REL]->d_un.d_ptr += l_addr;

# endif

		if (info[DT_JMPREL] != NULL)
			info[DT_JMPREL]->d_un.d_ptr += l_addr;

		if (info[VERSYMIDX(DT_VERSYM)] != NULL)
			info[VERSYMIDX(DT_VERSYM)]->d_un.d_ptr += l_addr;
	}

#endif

	if (info[DT_PLTREL] != NULL) {
#if ELF_MACHINE_NO_RELA
		assert(info[DT_PLTREL]->d_un.d_val == DT_REL);
#elif ELF_MACHINE_NO_REL
		assert(info[DT_PLTREL]->d_un.d_val == DT_RELA);
#else
		assert(info[DT_PLTREL]->d_un.d_val == DT_REL
		       || info[DT_PLTREL]->d_un.d_val == DT_RELA);
#endif
	}

#if ! ELF_MACHINE_NO_RELA

	if (info[DT_RELA] != NULL)
		assert(info[DT_RELAENT]->d_un.d_val == sizeof(ElfW(Rela)));

# endif
# if ! ELF_MACHINE_NO_REL

	if (info[DT_REL] != NULL)
		assert(info[DT_RELENT]->d_un.d_val == sizeof(ElfW(Rel)));

#endif

	if (info[DT_FLAGS] != NULL) {
		/* Flags are used.  Translate to the old form where available.
		 Since these l_info entries are only tested for NULL pointers it
		 is ok if they point to the DT_FLAGS entry.  */
		l->l_flags = info[DT_FLAGS]->d_un.d_val;
#ifdef RTLD_BOOTSTRAP
		/* These three flags must not be set for ld.so.  */
		assert((l->l_flags & (DF_SYMBOLIC | DF_TEXTREL | DF_BIND_NOW)) == 0);
#else

		if (l->l_flags & DF_SYMBOLIC)
			info[DT_SYMBOLIC] = info[DT_FLAGS];

		if (l->l_flags & DF_TEXTREL)
			info[DT_TEXTREL] = info[DT_FLAGS];

		if (l->l_flags & DF_BIND_NOW)
			info[DT_BIND_NOW] = info[DT_FLAGS];

#endif
	}

	if (info[VERSYMIDX(DT_FLAGS_1)] != NULL)
		l->l_flags_1 = info[VERSYMIDX(DT_FLAGS_1)]->d_un.d_val;

#ifdef RTLD_BOOTSTRAP
	/* The dynamic linker should have none of these set.  */
	assert(info[DT_RUNPATH] == NULL);
	assert(info[DT_RPATH] == NULL);
#else

	if (info[DT_RUNPATH] != NULL)
		/* If both RUNPATH and RPATH are given, the latter is ignored.  */
		info[DT_RPATH] = NULL;

#endif
}

#ifdef RESOLVE

# ifdef RTLD_BOOTSTRAP
#  define ELF_DURING_STARTUP (1)
# else
#  define ELF_DURING_STARTUP (0)
# endif

/* Get the definitions of `elf_dynamic_do_rel' and `elf_dynamic_do_rela'.
   These functions are almost identical, so we use cpp magic to avoid
   duplicating their code.  It cannot be done in a more general function
   because we must be able to completely inline.  */

/* On some machines, notably SPARC, DT_REL* includes DT_JMPREL in its
   range.  Note that according to the ELF spec, this is completely legal!
   But conditionally define things so that on machines we know this will
   not happen we do something more optimal.  */

# ifdef ELF_MACHINE_PLTREL_OVERLAP
#  define _ELF_DYNAMIC_DO_RELOC(RELOC, reloc, map, do_lazy, test_rel) \
  do {									      \
    struct { ElfW(Addr) start, size; int lazy; } ranges[3];		      \
    int ranges_index;							      \
									      \
    ranges[0].lazy = ranges[2].lazy = 0;				      \
    ranges[1].lazy = 1;							      \
    ranges[0].size = ranges[1].size = ranges[2].size = 0;		      \
									      \
    if ((map)->l_info[DT_##RELOC])					      \
      {									      \
	ranges[0].start = D_PTR ((map), l_info[DT_##RELOC]);		      \
	ranges[0].size = (map)->l_info[DT_##RELOC##SZ]->d_un.d_val;	      \
      }									      \
									      \
    if ((do_lazy)							      \
	&& (map)->l_info[DT_PLTREL]					      \
	&& (!test_rel || (map)->l_info[DT_PLTREL]->d_un.d_val == DT_##RELOC)) \
      {									      \
	ranges[1].start = D_PTR ((map), l_info[DT_JMPREL]);		      \
	ranges[1].size = (map)->l_info[DT_PLTRELSZ]->d_un.d_val;	      \
	ranges[2].start = ranges[1].start + ranges[1].size;		      \
	ranges[2].size = ranges[0].start + ranges[0].size - ranges[2].start;  \
	ranges[0].size = ranges[1].start - ranges[0].start;		      \
      }									      \
									      \
    for (ranges_index = 0; ranges_index < 3; ++ranges_index)		      \
      elf_dynamic_do_##reloc ((map),					      \
			      ranges[ranges_index].start,		      \
			      ranges[ranges_index].size,		      \
			      ranges[ranges_index].lazy);		      \
  } while (0)
# else
#  define _ELF_DYNAMIC_DO_RELOC(RELOC, reloc, map, do_lazy, test_rel) \
  do {									      \
    struct { ElfW(Addr) start, size; int lazy; } ranges[2];		      \
    ranges[0].lazy = 0;							      \
    ranges[0].size = ranges[1].size = 0;				      \
    ranges[0].start = 0;						      \
									      \
    if ((map)->l_info[DT_##RELOC])					      \
      {									      \
	ranges[0].start = D_PTR ((map), l_info[DT_##RELOC]);		      \
	ranges[0].size = (map)->l_info[DT_##RELOC##SZ]->d_un.d_val;	      \
      }									      \
    if ((map)->l_info[DT_PLTREL]					      \
	&& (!test_rel || (map)->l_info[DT_PLTREL]->d_un.d_val == DT_##RELOC)) \
      {									      \
	ElfW(Addr) start = D_PTR ((map), l_info[DT_JMPREL]);		      \
									      \
	if (! ELF_DURING_STARTUP					      \
	    && ((do_lazy)						      \
		/* This test does not only detect whether the relocation      \
		   sections are in the right order, it also checks whether    \
		   there is a DT_REL/DT_RELA section.  */		      \
		|| ranges[0].start + ranges[0].size != start))		      \
	  {								      \
	    ranges[1].start = start;					      \
	    ranges[1].size = (map)->l_info[DT_PLTRELSZ]->d_un.d_val;	      \
	    ranges[1].lazy = (do_lazy);					      \
	  }								      \
	else								      \
	  {								      \
	    /* Combine processing the sections.  */			      \
	    assert (ranges[0].start + ranges[0].size == start);		      \
	    ranges[0].size += (map)->l_info[DT_PLTRELSZ]->d_un.d_val;	      \
	  }								      \
      }									      \
									      \
    if (ELF_DURING_STARTUP)						      \
      elf_dynamic_do_##reloc ((map), ranges[0].start, ranges[0].size, 0);     \
    else								      \
      {									      \
	int ranges_index;						      \
	for (ranges_index = 0; ranges_index < 2; ++ranges_index)	      \
	  elf_dynamic_do_##reloc ((map),				      \
				  ranges[ranges_index].start,		      \
				  ranges[ranges_index].size,		      \
				  ranges[ranges_index].lazy);		      \
      }									      \
  } while (0)
# endif

# if ELF_MACHINE_NO_REL || ELF_MACHINE_NO_RELA
#  define _ELF_CHECK_REL 0
# else
#  define _ELF_CHECK_REL 1
# endif

# if ! ELF_MACHINE_NO_REL
#  include "do-rel.h"
#  define ELF_DYNAMIC_DO_REL(map, lazy) \
  _ELF_DYNAMIC_DO_RELOC (REL, rel, map, lazy, _ELF_CHECK_REL)
# else
#  define ELF_DYNAMIC_DO_REL(map, lazy) /* Nothing to do.  */
# endif

# if ! ELF_MACHINE_NO_RELA
#  define DO_RELA
#  include "do-rel.h"
#  define ELF_DYNAMIC_DO_RELA(map, lazy) \
  _ELF_DYNAMIC_DO_RELOC (RELA, rela, map, lazy, _ELF_CHECK_REL)
# else
#  define ELF_DYNAMIC_DO_RELA(map, lazy) /* Nothing to do.  */
# endif

/* This can't just be an inline function because GCC is too dumb
   to inline functions containing inlines themselves.  */
# define ELF_DYNAMIC_RELOCATE(map, lazy, consider_profile) \
  do {									      \
    int edr_lazy = elf_machine_runtime_setup ((map), (lazy),		      \
					      (consider_profile));	      \
    ELF_DYNAMIC_DO_REL ((map), edr_lazy);				      \
    ELF_DYNAMIC_DO_RELA ((map), edr_lazy);				      \
  } while (0)

#endif
