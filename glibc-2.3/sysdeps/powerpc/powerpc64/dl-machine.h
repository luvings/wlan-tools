/* Machine-dependent ELF dynamic relocation inline functions.
   PowerPC64 version.
   Copyright 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "powerpc64"

#include <assert.h>
#include <sys/param.h>

/* Translate a processor specific dynamic tag to the index
   in l_info array.  */
#define DT_PPC64(x) (DT_PPC64_##x - DT_LOPROC + DT_NUM)

/* A PowerPC64 function descriptor.  The .plt (procedure linkage
   table) and .opd (official procedure descriptor) sections are
   arrays of these.  */
typedef struct {
	Elf64_Addr fd_func;
	Elf64_Addr fd_toc;
	Elf64_Addr fd_aux;
} Elf64_FuncDesc;

#define ELF_MULT_MACHINES_SUPPORTED

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int elf_machine_matches_host(const Elf64_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_PPC64;
}

/* Return nonzero iff ELF header is compatible with the running host,
   but not this loader.  */
static inline int elf_host_tolerates_machine(const Elf64_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_PPC;
}

/* Return nonzero iff ELF header is compatible with the running host,
   but not this loader.  */
static inline int elf_host_tolerates_class(const Elf64_Ehdr *ehdr)
{
	return ehdr->e_ident[EI_CLASS] == ELFCLASS32;
}


/* Return the run-time load address of the shared object, assuming it
   was originally linked at zero.  */
static inline Elf64_Addr elf_machine_load_address(void) __attribute__((const));

static inline Elf64_Addr elf_machine_load_address(void)
{
	Elf64_Addr ret;

	/* The first entry in .got (and thus the first entry in .toc) is the
	   link-time TOC_base, ie. r2.  So the difference between that and
	   the current r2 set by the kernel is how far the shared lib has
	   moved.  */
	asm("	ld	%0,-32768(2)\n"
	    "	subf	%0,%0,2\n"
	    : "=r"(ret));
	return ret;
}

/* Return the link-time address of _DYNAMIC.  */
static inline Elf64_Addr elf_machine_dynamic(void)
{
	Elf64_Addr runtime_dynamic;
	/* It's easier to get the run-time address.  */
	asm("	addis	%0,2,_DYNAMIC@toc@ha\n"
	    "	addi	%0,%0,_DYNAMIC@toc@l\n"
	    : "=b"(runtime_dynamic));
	/* Then subtract off the load address offset.  */
	return runtime_dynamic - elf_machine_load_address() ;
}

#define ELF_MACHINE_BEFORE_RTLD_RELOC(dynamic_info) /* nothing */

/* The PLT uses Elf64_Rela relocs.  */
#define elf_machine_relplt elf_machine_rela

/* This code gets called via a .glink stub which loads PLT0.  It is
   used in dl-runtime.c to call the `fixup' function and then redirect
   to the address `fixup' returns.

   Enter with r0 = plt reloc index,
   r2 = ld.so tocbase,
   r11 = ld.so link map.  */

#define TRAMPOLINE_TEMPLATE(tramp_name, fixup_name) \
  asm (".section \".text\"\n"						\
"	.align	2\n"							\
"	.globl	." #tramp_name "\n"					\
"	.type	." #tramp_name ",@function\n"				\
"	.section \".opd\",\"aw\"\n"					\
"	.align	3\n"							\
"	.globl	" #tramp_name "\n"					\
"	.size	" #tramp_name ",24\n"					\
#tramp_name ":\n"							\
"	.quad	." #tramp_name ",.TOC.@tocbase,0\n"			\
"	.previous\n"							\
"." #tramp_name ":\n"							\
/* We need to save the registers used to pass parameters, ie. r3 thru	\
   r10; the registers are saved in a stack frame.  */			\
"	stdu	1,-128(1)\n"						\
"	std	3,48(1)\n"						\
"	mr	3,11\n"							\
"	std	4,56(1)\n"						\
"	sldi	4,0,1\n"						\
"	std	5,64(1)\n"						\
"	add	4,4,0\n"						\
"	std	6,72(1)\n"						\
"	sldi	4,4,3\n"						\
"	std	7,80(1)\n"						\
"	mflr	0\n"							\
"	std	8,88(1)\n"						\
/* Store the LR in the LR Save area of the previous frame.  */    \
"	std	0,128+16(1)\n"						\
"	mfcr	0\n"							\
"	std	9,96(1)\n"						\
"	std	10,104(1)\n"						\
/* I'm almost certain we don't have to save cr...  be safe.  */    \
"	std	0,8(1)\n"						\
"	bl	." #fixup_name "\n"					\
/* Put the registers back.  */						\
"	ld	0,128+16(1)\n"						\
"	ld	10,104(1)\n"						\
"	ld	9,96(1)\n"						\
"	ld	8,88(1)\n"						\
"	ld	7,80(1)\n"						\
"	mtlr	0\n"							\
"	ld	0,8(1)\n"						\
"	ld	6,72(1)\n"						\
"	ld	5,64(1)\n"						\
"	ld	4,56(1)\n"						\
"	mtcrf	0xFF,0\n"						\
/* Load the target address, toc and static chain reg from the function  \
   descriptor returned by fixup.  */					\
"	ld	0,0(3)\n"						\
"	ld	2,8(3)\n"						\
"	mtctr	0\n"							\
"	ld	11,16(3)\n"						\
"	ld	3,48(1)\n"						\
/* Unwind the stack frame, and jump.  */				\
"	addi	1,1,128\n"						\
"	bctr\n"								\
".LT_" #tramp_name ":\n"						\
"	.long 0\n"							\
"	.byte 0x00,0x0c,0x24,0x40,0x00,0x00,0x00,0x00\n"		\
"	.long .LT_" #tramp_name "-."#tramp_name "\n"			\
"	.short .LT_" #tramp_name "_name_end-.LT_" #tramp_name "_name_start\n" \
".LT_" #tramp_name "_name_start:\n"					\
"	.ascii \"" #tramp_name "\"\n"					\
".LT_" #tramp_name "_name_end:\n"					\
"	.align 2\n"							\
"	.size	." #tramp_name ",. - ." #tramp_name "\n"		\
"	.previous");

#ifndef PROF
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup);	\
  TRAMPOLINE_TEMPLATE (_dl_profile_resolve, profile_fixup);
#else
#define ELF_MACHINE_RUNTIME_TRAMPOLINE			\
  TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup);	\
  void _dl_runtime_resolve (void);			\
  strong_alias (_dl_runtime_resolve, _dl_profile_resolve);
#endif


/* Initial entry point code for the dynamic linker.  The C function
   `_dl_start' is the real entry point; its return value is the user
   program's entry point.  */
#define RTLD_START \
  asm (".section \".text\"\n"						\
"	.align	2\n"							\
"	.globl	._start\n"						\
"	.type	._start,@function\n"					\
"	.section \".opd\",\"aw\"\n"					\
"	.align	3\n"							\
"	.globl	_start\n"						\
"	.size	_start,24\n"						\
"_start:\n"								\
"	.quad	._start,.TOC.@tocbase,0\n"				\
"	.previous\n"							\
"._start:\n"								\
/* We start with the following on the stack, from top:			\
   argc (4 bytes);							\
   arguments for program (terminated by NULL);				\
   environment variables (terminated by NULL);				\
   arguments for the program loader.  */				\
"	mr	3,1\n"							\
"	li	4,0\n"							\
"	stdu	4,-128(1)\n"						\
/* Call _dl_start with one parameter pointing at argc.  */		\
"	bl	._dl_start\n"						\
"	nop\n"								\
/* Transfer control to _dl_start_user!  */				\
"	b	._dl_start_user\n"					\
".LT__start:\n"  \
"	.long 0\n"      \
"	.byte 0x00,0x0c,0x24,0x40,0x00,0x00,0x00,0x00\n"		\
"	.long .LT__start-._start\n"					\
"	.short .LT__start_name_end-.LT__start_name_start\n"		\
".LT__start_name_start:\n"						\
"	.ascii \"_start\"\n"						\
".LT__start_name_end:\n"						\
"	.align 2\n"							\
"	.size	._start,.-._start\n"					\
"	.globl	_dl_start_user\n"					\
"	.section \".opd\",\"aw\"\n"					\
"_dl_start_user:\n"							\
"	.quad	._dl_start_user, .TOC.@tocbase, 0\n"			\
"	.previous\n"							\
"	.globl	._dl_start_user\n"					\
"	.type	._dl_start_user,@function\n"				\
/* Now, we do our main work of calling initialisation procedures.  \
   The ELF ABI doesn't say anything about parameters for these,  \
   so we just pass argc, argv, and the environment.  \
   Changing these is strongly discouraged (not least because argc is  \
   passed by value!).  */  \
"._dl_start_user:\n"  \
/* the address of _start in r30.  */					\
"	mr	30,3\n"							\
/* &_dl_argc in 29, &_dl_argv in 27, and _dl_loaded in 28.  */		\
"	ld	28,_rtld_global@got(2)\n"    \
"	ld	29,_dl_argc@got(2)\n"					\
"	ld	27,_dl_argv@got(2)\n"					\
/* _dl_init (_dl_loaded, _dl_argc, _dl_argv, _dl_argv+_dl_argc+1).  */	\
"	ld	3,0(28)\n"						\
"	lwa	4,0(29)\n"						\
"	ld	5,0(27)\n"						\
"	sldi	6,4,3\n"						\
"	add	6,5,6\n"						\
"	addi	6,6,8\n"						\
"	bl	._dl_init\n"						\
"	nop\n"								\
/* Now, to conform to the ELF ABI, we have to:				\
   Pass argc (actually _dl_argc) in r3;  */				\
"	lwa	3,0(29)\n"						\
/* Pass argv (actually _dl_argv) in r4;  */				\
"	ld	4,0(27)\n"						\
/* Pass argv+argc+1 in r5;  */						\
"	sldi	5,3,3\n"						\
"	add	6,4,5\n"						\
"	addi	5,6,8\n"						\
/* Pass the auxilary vector in r6. This is passed to us just after	\
   _envp.  */								\
"2:	ldu	0,8(6)\n"						\
"	cmpdi	0,0\n"							\
"	bne	2b\n"							\
"	addi	6,6,8\n"						\
/* Pass a termination function pointer (in this case _dl_fini) in	\
   r7.  */								\
"	ld	7,_dl_fini@got(2)\n"					\
"	ld 	26,_dl_starting_up@got(2)\n"				\
/* Pass the stack pointer in r1 (so far so good), pointing to a NULL	\
   value.  This lets our startup code distinguish between a program	\
   linked statically, which linux will call with argc on top of the	\
   stack which will hopefully never be zero, and a dynamically linked	\
   program which will always have a NULL on the top of the stack.	\
   Take the opportunity to clear LR, so anyone who accidentally  \
   returns from _start gets SEGV.  Also clear the next few words of	\
   the stack.  */							\
"	li	31,0\n"							\
"	std	31,0(1)\n"						\
"	mtlr	31\n"							\
"	std	31,8(1)\n"						\
"	std	31,16(1)\n"						\
"	std	31,24(1)\n"						\
/* Clear _dl_starting_up.  */						\
"	stw	31,0(26)\n"						\
/* Now, call the start function descriptor at r30...  */		\
"	.globl	._dl_main_dispatch\n"  \
"._dl_main_dispatch:\n"  \
"	ld	0,0(30)\n"						\
"	ld	2,8(30)\n"						\
"	mtctr	0\n"							\
"	ld	11,16(30)\n"						\
"	bctr\n"								\
".LT__dl_start_user:\n"	\
"	.long 0\n"							\
"	.byte 0x00,0x0c,0x24,0x40,0x00,0x00,0x00,0x00\n"		\
"	.long .LT__dl_start_user-._dl_start_user\n"			\
"	.short .LT__dl_start_user_name_end-.LT__dl_start_user_name_start\n" \
".LT__dl_start_user_name_start:\n"					\
"	.ascii \"_dl_start_user\"\n"					\
".LT__dl_start_user_name_end:\n"					\
"	.align 2\n"							\
"	.size	._dl_start_user,.-._dl_start_user\n"			\
"	.previous");

/* Nonzero iff TYPE should not be allowed to resolve to one of
   the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_lookup_noexec_p(type) ((type) == R_PPC64_COPY)

/* Nonzero iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.  */
#define elf_machine_lookup_noplt_p(type) ((type) == R_PPC64_JMP_SLOT)

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_PPC64_ADDR24) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_PPC64_COPY) * ELF_RTYPE_CLASS_COPY))

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_PPC64_JMP_SLOT

/* The PowerPC never uses REL relocations.  */
#define ELF_MACHINE_NO_REL 1

/* Stuff for the PLT.  */
#define PLT_INITIAL_ENTRY_WORDS 3
#define GLINK_INITIAL_ENTRY_WORDS 8

#define PPC_DCBST(where) asm volatile ("dcbst 0,%0" : : "r"(where) : "memory")
#define PPC_SYNC asm volatile ("sync" : : : "memory")
#define PPC_ISYNC asm volatile ("sync; isync" : : : "memory")
#define PPC_ICBI(where) asm volatile ("icbi 0,%0" : : "r"(where) : "memory")
#define PPC_DIE asm volatile ("tweq 0,0")
/* Use this when you've modified some code, but it won't be in the
   instruction fetch queue (or when it doesn't matter if it is). */
#define MODIFIED_CODE_NOQUEUE(where) \
     do { PPC_DCBST(where); PPC_SYNC; PPC_ICBI(where); } while (0)
/* Use this when it might be in the instruction queue. */
#define MODIFIED_CODE(where) \
     do { PPC_DCBST(where); PPC_SYNC; PPC_ICBI(where); PPC_ISYNC; } while (0)

/* Set up the loaded object described by MAP so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */
static inline int elf_machine_runtime_setup(struct link_map *map, int lazy, int profile)
{
	if (map->l_info[DT_JMPREL]) {
		Elf64_Word i;
		Elf64_Word *glink = NULL;
		Elf64_Xword *plt = (Elf64_Xword *) D_PTR(map, l_info[DT_PLTGOT]);
		Elf64_Word num_plt_entries = (map->l_info[DT_PLTRELSZ]->d_un.d_val
		                              / sizeof(Elf64_Rela));
		Elf64_Addr l_addr = map->l_addr;
		Elf64_Dyn **info = map->l_info;
		char *p;

		extern void _dl_runtime_resolve(void);
		extern void _dl_profile_resolve(void);

		/* Relocate the DT_PPC64_GLINK entry in the _DYNAMIC section.
		 elf_get_dynamic_info takes care of the standard entries but
		 doesn't know exactly what to do with processor specific
		 entires.  */
		if (info[DT_PPC64(GLINK)] != NULL)
			info[DT_PPC64(GLINK)]->d_un.d_ptr += l_addr;

		if (lazy) {
			/* The function descriptor of the appropriate trampline
			   routine is used to set the 1st and 2nd doubleword of the
			   plt_reserve.  */
			Elf64_FuncDesc *resolve_fd;
			Elf64_Word glink_offset;
			/* the plt_reserve area is the 1st 3 doublewords of the PLT */
			Elf64_FuncDesc *plt_reserve = (Elf64_FuncDesc *) plt;
			Elf64_Word offset;

			resolve_fd = (Elf64_FuncDesc *)(profile ? _dl_profile_resolve
			                                : _dl_runtime_resolve);

			if (profile && _dl_name_match_p(GL(dl_profile), map))
				/* This is the object we are looking for.  Say that we really
				   want profiling and the timers are started.  */
				GL(dl_profile_map) = map;


			/* We need to stuff the address/TOC of _dl_runtime_resolve
			   into doublewords 0 and 1 of plt_reserve.  Then we need to
			   stuff the map address into doubleword 2 of plt_reserve.
			   This allows the GLINK0 code to transfer control to the
			   correct trampoline which will transfer control to fixup
			   in dl-machine.c.  */
			plt_reserve->fd_func = resolve_fd->fd_func;
			plt_reserve->fd_toc  = resolve_fd->fd_toc;
			plt_reserve->fd_aux  = (Elf64_Addr) map;
#ifdef RTLD_BOOTSTRAP
			/* When we're bootstrapping, the opd entry will not have
			   been relocated yet.  */
			plt_reserve->fd_func += l_addr;
			plt_reserve->fd_toc  += l_addr;
#endif

			/* Set up the lazy PLT entries.  */
			glink = (Elf64_Word *) D_PTR(map, l_info[DT_PPC64(GLINK)]);
			offset = PLT_INITIAL_ENTRY_WORDS;
			glink_offset = GLINK_INITIAL_ENTRY_WORDS;

			for (i = 0; i < num_plt_entries; i++) {

				plt[offset] = (Elf64_Xword) &glink[glink_offset];
				offset += 3;

				/* The first 32k entries of glink can set an index and
				 branch using two instructions;  Past that point,
				 glink uses three instructions.  */
				if (i < 0x8000)
					glink_offset += 2;

				else
					glink_offset += 3;
			}

			/* Now, we've modified data.  We need to write the changes from
			   the data cache to a second-level unified cache, then make
			   sure that stale data in the instruction cache is removed.
			   (In a multiprocessor system, the effect is more complex.)
			   Most of the PLT shouldn't be in the instruction cache, but
			   there may be a little overlap at the start and the end.

			   Assumes that dcbst and icbi apply to lines of 16 bytes or
			   more.  Current known line sizes are 16, 32, and 128 bytes.  */

			for (p = (char *) plt; p < (char *) &plt[offset]; p += 16)
				PPC_DCBST(p);

			PPC_SYNC;
		}
	}

	return lazy;
}

static inline void elf_machine_lazy_rel(struct link_map *map,
                                        Elf64_Addr l_addr, const Elf64_Rela *reloc)
{
	/* elf_machine_runtime_setup handles this.  */
}

/* Change the PLT entry whose reloc is 'reloc' to call the actual
   routine.  */
static inline Elf64_Addr elf_machine_fixup_plt(struct link_map *map, lookup_t sym_map,
                const Elf64_Rela *reloc,
                Elf64_Addr *reloc_addr, Elf64_Addr finaladdr)
{
	Elf64_FuncDesc *plt = (Elf64_FuncDesc *) reloc_addr;
	Elf64_FuncDesc *rel = (Elf64_FuncDesc *) finaladdr;
	Elf64_Addr offset = 0;
#ifndef RTLD_BOOTSTRAP
	weak_extern(GL(dl_rtld_map));
#endif

	/* If sym_map is NULL, it's a weak undefined sym;  Leave the plt zero.  */
	if (sym_map == NULL)
		return 0;

	/* If the opd entry is not yet relocated (because it's from a shared
	   object that hasn't been processed yet), then manually reloc it.  */
	if (map != sym_map && !sym_map->l_relocated
#ifndef RTLD_BOOTSTRAP
	                /* Bootstrap map doesn't have l_relocated set for it.  */
	                && sym_map != &GL(dl_rtld_map)
#endif
	   )
		offset = sym_map->l_addr;

	/* For PPC64, fixup_plt copies the function descriptor from opd
	   over the corresponding PLT entry.
	   Initially, PLT Entry[i] is set up for lazy linking, or is zero.
	   For lazy linking, the fd_toc and fd_aux entries are irrelevant,
	   so for thread safety we write them before changing fd_func.  */

	plt->fd_aux = rel->fd_aux + offset;
	plt->fd_toc = rel->fd_toc + offset;
	PPC_DCBST(&plt->fd_aux);
	PPC_DCBST(&plt->fd_toc);
	PPC_SYNC;

	plt->fd_func = rel->fd_func + offset;
	PPC_DCBST(&plt->fd_func);
	PPC_SYNC;

	return finaladdr;
}

/* Return the final value of a plt relocation.  */
static inline Elf64_Addr elf_machine_plt_value(struct link_map *map, const Elf64_Rela *reloc,
                Elf64_Addr value)
{
	return value + reloc->r_addend;
}

#endif /* dl_machine_h */

#ifdef RESOLVE_MAP

#define PPC_LO(v) ((v) & 0xffff)
#define PPC_HI(v) (((v) >> 16) & 0xffff)
#define PPC_HA(v) PPC_HI ((v) + 0x8000)
#define PPC_HIGHER(v) (((v) >> 32) & 0xffff)
#define PPC_HIGHERA(v) PPC_HIGHER ((v) + 0x8000)
#define PPC_HIGHEST(v) (((v) >> 48) & 0xffff)
#define PPC_HIGHESTA(v) PPC_HIGHEST ((v) + 0x8000)
#define BIT_INSERT(old, val, mask) ((old & ~(Elf64_Addr) mask) | (val & mask))

#define dont_expect(X) __builtin_expect ((X), 0)

extern void _dl_reloc_overflow(struct link_map *map,
                               const char *name,
                               Elf64_Addr *const reloc_addr,
                               const Elf64_Sym *sym,
                               const Elf64_Sym *refsym)
attribute_hidden;

static inline void elf_machine_rela_relative(Elf64_Addr l_addr, const Elf64_Rela *reloc,
                Elf64_Addr *const reloc_addr)
{
	*reloc_addr = l_addr + reloc->r_addend;
}

/* Perform the relocation specified by RELOC and SYM (which is fully
   resolved).  MAP is the object containing the reloc.  */
static inline void elf_machine_rela(struct link_map *map,
                                    const Elf64_Rela *reloc,
                                    const Elf64_Sym *sym,
                                    const struct r_found_version *version,
                                    Elf64_Addr *const reloc_addr)
{
	int r_type = ELF64_R_TYPE(reloc->r_info);
	struct link_map *sym_map;
	Elf64_Addr value;
#ifndef RTLD_BOOTSTRAP
	const Elf64_Sym *const refsym = sym;
	/* This is defined in rtld.c, but nowhere in the static libc.a; make the
	   reference weak so static programs can still link.  This declaration
	   cannot be done when compiling rtld.c (i.e.  #ifdef RTLD_BOOTSTRAP)
	   because rtld.c contains the common defn for _dl_rtld_map, which is
	   incompatible with a weak decl in the same file.  */
	weak_extern(GL(dl_rtld_map));
#endif

	if (r_type == R_PPC64_RELATIVE) {
#ifndef RTLD_BOOTSTRAP

		/* Already done in dynamic linker.  */
		if (map != &GL(dl_rtld_map))
#endif
			*reloc_addr = map->l_addr + reloc->r_addend;

		return;
	}

	if (r_type == R_PPC64_NONE)
		return;

	sym_map = RESOLVE_MAP(&sym, version, r_type);
	value = 0;

	if (sym_map) {
		if (sym)
			value = sym_map->l_addr + sym->st_value;

		value += reloc->r_addend;
	}

	switch (r_type) {
	case R_PPC64_ADDR64:
	case R_PPC64_GLOB_DAT:
		*reloc_addr = value;
		return;

	case R_PPC64_JMP_SLOT:

		elf_machine_fixup_plt(map, sym_map, reloc, reloc_addr, value);
		return;

#ifndef RTLD_BOOTSTRAP /* None of the following appear in ld.so */

	case R_PPC64_ADDR16_LO_DS:
		if (dont_expect((value & 3) != 0))
			_dl_reloc_overflow(map, "R_PPC64_ADDR16_LO_DS",
			                   reloc_addr, sym, refsym);

		*(Elf64_Half *) reloc_addr = BIT_INSERT(*(Elf64_Half *) reloc_addr,
		                                        value, 0xfffc);
		break;

	case R_PPC64_ADDR16_LO:
		*(Elf64_Half *) reloc_addr = PPC_LO(value);
		break;

	case R_PPC64_ADDR16_HI:
		*(Elf64_Half *) reloc_addr = PPC_HI(value);
		break;

	case R_PPC64_ADDR16_HA:
		*(Elf64_Half *) reloc_addr = PPC_HA(value);
		break;

	case R_PPC64_REL24: {
		Elf64_Addr delta = value - (Elf64_Xword) reloc_addr;

		if (dont_expect((delta + 0x2000000) >= 0x4000000 || (delta & 3) != 0))
			_dl_reloc_overflow(map, "R_PPC64_REL24", reloc_addr, sym, refsym);

		*(Elf64_Word *) reloc_addr = BIT_INSERT(*(Elf64_Word *) reloc_addr,
		                                        delta, 0x3fffffc);
	}
	break;

	case R_PPC64_COPY:
		if (dont_expect(sym == NULL))
			/* This can happen in trace mode when an object could not be found. */
			return;

		if (dont_expect(sym->st_size > refsym->st_size
		                || (GL(dl_verbose) && sym->st_size < refsym->st_size))) {
			const char *strtab;

			strtab = (const void *) D_PTR(map, l_info[DT_STRTAB]);
			_dl_error_printf("%s: Symbol `%s' has different size" \
			                 " in shared object," \
			                 " consider re-linking\n",
			                 _dl_argv[0] ? : "<program name unknown>",
			                 strtab + refsym->st_name);
		}

		memcpy(reloc_addr, (char *) value, MIN(sym->st_size, refsym->st_size));
		return;

	case R_PPC64_UADDR64:
		/* We are big-endian.  */
		((char *) reloc_addr)[0] = (value >> 56) & 0xff;
		((char *) reloc_addr)[1] = (value >> 48) & 0xff;
		((char *) reloc_addr)[2] = (value >> 40) & 0xff;
		((char *) reloc_addr)[3] = (value >> 32) & 0xff;
		((char *) reloc_addr)[4] = (value >> 24) & 0xff;
		((char *) reloc_addr)[5] = (value >> 16) & 0xff;
		((char *) reloc_addr)[6] = (value >> 8) & 0xff;
		((char *) reloc_addr)[7] = (value >> 0) & 0xff;
		return;

	case R_PPC64_UADDR32:
		/* We are big-endian.  */
		((char *) reloc_addr)[0] = (value >> 24) & 0xff;
		((char *) reloc_addr)[1] = (value >> 16) & 0xff;
		((char *) reloc_addr)[2] = (value >> 8) & 0xff;
		((char *) reloc_addr)[3] = (value >> 0) & 0xff;
		return;

	case R_PPC64_ADDR24:
		if (dont_expect((value + 0x2000000) >= 0x4000000 || (value & 3) != 0))
			_dl_reloc_overflow(map, "R_PPC64_ADDR24", reloc_addr, sym, refsym);

		*(Elf64_Word *) reloc_addr = BIT_INSERT(*(Elf64_Word *) reloc_addr,
		                                        value, 0x3fffffc);
		break;

	case R_PPC64_ADDR16:
		if (dont_expect((value + 0x8000) >= 0x10000))
			_dl_reloc_overflow(map, "R_PPC64_ADDR16", reloc_addr, sym, refsym);

		*(Elf64_Half *) reloc_addr = value;
		break;

	case R_PPC64_UADDR16:
		if (dont_expect((value + 0x8000) >= 0x10000))
			_dl_reloc_overflow(map, "R_PPC64_UADDR16", reloc_addr, sym, refsym);

		/* We are big-endian.  */
		((char *) reloc_addr)[0] = (value >> 8) & 0xff;
		((char *) reloc_addr)[1] = (value >> 0) & 0xff;
		break;

	case R_PPC64_ADDR16_DS:
		if (dont_expect((value + 0x8000) >= 0x10000 || (value & 3) != 0))
			_dl_reloc_overflow(map, "R_PPC64_ADDR16_DS", reloc_addr, sym, refsym);

		*(Elf64_Half *) reloc_addr = BIT_INSERT(*(Elf64_Half *) reloc_addr,
		                                        value, 0xfffc);
		break;

	case R_PPC64_ADDR16_HIGHER:
		*(Elf64_Half *) reloc_addr = PPC_HIGHER(value);
		break;

	case R_PPC64_ADDR16_HIGHEST:
		*(Elf64_Half *) reloc_addr = PPC_HIGHEST(value);
		break;

	case R_PPC64_ADDR16_HIGHERA:
		*(Elf64_Half *) reloc_addr = PPC_HIGHERA(value);
		break;

	case R_PPC64_ADDR16_HIGHESTA:
		*(Elf64_Half *) reloc_addr = PPC_HIGHESTA(value);
		break;

	case R_PPC64_ADDR14:
	case R_PPC64_ADDR14_BRTAKEN:
	case R_PPC64_ADDR14_BRNTAKEN: {
		Elf64_Word insn;

		if (dont_expect((value + 0x8000) >= 0x10000 || (value & 3) != 0))
			_dl_reloc_overflow(map, "R_PPC64_ADDR14", reloc_addr, sym, refsym);

		insn = BIT_INSERT(*(Elf64_Word *) reloc_addr, value, 0xfffc);

		if (r_type != R_PPC64_ADDR14) {
			insn &= ~(1 << 21);

			if (r_type == R_PPC64_ADDR14_BRTAKEN)
				insn |= 1 << 21;

			if ((insn & (0x14 << 21)) == (0x04 << 21))
				insn |= 0x02 << 21;

			else if ((insn & (0x14 << 21)) == (0x10 << 21))
				insn |= 0x08 << 21;
		}

		*(Elf64_Word *) reloc_addr = insn;
	}
	break;

	case R_PPC64_REL32:
		*(Elf64_Word *) reloc_addr = value - (Elf64_Xword) reloc_addr;
		return;
#endif /* !RTLD_BOOTSTRAP */

	default:
		_dl_reloc_bad_type(map, r_type, 0);
		return;
	}

	MODIFIED_CODE_NOQUEUE(reloc_addr);
}

#endif /* RESOLVE */
