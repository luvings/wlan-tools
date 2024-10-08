/* Machine-dependent ELF dynamic relocation inline functions.  m68k version.
   Copyright (C) 1996-2001, 2002 Free Software Foundation, Inc.
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

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "m68k"

#include <sys/param.h>

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int elf_machine_matches_host(const Elf32_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_68K;
}


/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
   first element of the GOT.  This must be inlined in a function which
   uses global data.  */
static inline Elf32_Addr elf_machine_dynamic(void)
{
	register Elf32_Addr *got asm("%a5");
	return *got;
}


/* Return the run-time load address of the shared object.  */
static inline Elf32_Addr elf_machine_load_address(void)
{
	Elf32_Addr addr;
	asm("lea _dl_start(%%pc), %0\n\t"
	    "sub.l _dl_start@GOT.w(%%a5), %0"
	    : "=a"(addr));
	return addr;
}


/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */

static inline int elf_machine_runtime_setup(struct link_map *l, int lazy, int profile)
{
	Elf32_Addr *got;
	extern void _dl_runtime_resolve(Elf32_Word);
	extern void _dl_runtime_profile(Elf32_Word);

	if (l->l_info[DT_JMPREL] && lazy) {
		/* The GOT entries for functions in the PLT have not yet been
		 filled in.  Their initial contents will arrange when called
		 to push an offset into the .rela.plt section, push
		 _GLOBAL_OFFSET_TABLE_[1], and then jump to
		 _GLOBAL_OFFSET_TABLE_[2].  */
		got = (Elf32_Addr *) D_PTR(l, l_info[DT_PLTGOT]);
		got[1] = (Elf32_Addr) l;	/* Identify this shared object.  */

		/* The got[2] entry contains the address of a function which gets
		 called to get the address of a so far unresolved function and
		 jump to it.  The profiling extension of the dynamic linker allows
		 to intercept the calls to collect information.  In this case we
		 don't store the address in the GOT so that all future calls also
		 end in this function.  */
		if (profile) {
			got[2] = (Elf32_Addr) &_dl_runtime_profile;

			if (_dl_name_match_p(GL(dl_profile), l)) {
				/* This is the object we are looking for.  Say that we really
				 want profiling and the timers are started.  */
				GL(dl_profile_map) = l;
			}

		} else
			/* This function will get called to fix up the GOT entry indicated by
			   the offset on the stack, and then jump to the resolved address.  */
			got[2] = (Elf32_Addr) &_dl_runtime_resolve;
	}

	return lazy;
}

/* This code is used in dl-runtime.c to call the `fixup' function
   and then redirect to the address it returns.  */
#define TRAMPOLINE_TEMPLATE(tramp_name, fixup_name) \
"| Trampoline for " #fixup_name "\n\
	.globl " #tramp_name "\n\
	.type " #tramp_name ", @function\n\
" #tramp_name ":\n\
	| Save %a0 (struct return address) and %a1.\n\
	move.l %a0, -(%sp)\n\
	move.l %a1, -(%sp)\n\
	| Call the real address resolver.\n\
	jbsr " #fixup_name "\n\
	| Restore register %a0 and %a1.\n\
	move.l (%sp)+, %a1\n\
	move.l (%sp)+, %a0\n\
	| Pop parameters\n\
	addq.l #8, %sp\n\
	| Call real function.\n\
	jmp (%d0)\n\
	.size " #tramp_name ", . - " #tramp_name "\n"
#ifndef PROF
#define ELF_MACHINE_RUNTIME_TRAMPOLINE \
asm (TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup) \
     TRAMPOLINE_TEMPLATE (_dl_runtime_profile, profile_fixup));
#else
#define ELF_MACHINE_RUNTIME_TRAMPOLINE \
asm (TRAMPOLINE_TEMPLATE (_dl_runtime_resolve, fixup) \
     ".globl _dl_runtime_profile\n" \
     ".set _dl_runtime_profile, _dl_runtime_resolve");
#endif
#define ELF_MACHINE_RUNTIME_FIXUP_ARGS long int save_a0, long int save_a1


/* Mask identifying addresses reserved for the user program,
   where the dynamic linker should not map anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0x80000000UL

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */

#define RTLD_START asm ("\
	.text\n\
	.globl _start\n\
	.type _start,@function\n\
_start:\n\
	move.l %sp, -(%sp)\n\
	jbsr _dl_start\n\
	addq.l #4, %sp\n\
	/* FALLTHRU */\n\
\n\
	.globl _dl_start_user\n\
	.type _dl_start_user,@function\n\
_dl_start_user:\n\
	| Save the user entry point address in %a4.\n\
	move.l %d0, %a4\n\
	| Point %a5 at the GOT.\n\
	lea _GLOBAL_OFFSET_TABLE_@GOTPC(%pc), %a5\n\
	| Remember the highest stack address.\n\
	move.l %sp, ([__libc_stack_end@GOT.w, %a5])\n\
	| See if we were run as a command with the executable file\n\
	| name as an extra leading argument.\n\
	move.l ([_dl_skip_args@GOT.w, %a5]), %d0\n\
	| Pop the original argument count\n\
	move.l (%sp)+, %d1\n\
	| Subtract _dl_skip_args from it.\n\
	sub.l %d0, %d1\n\
	| Adjust the stack pointer to skip _dl_skip_args words.\n\
	lea (%sp, %d0*4), %sp\n\
	| Push back the modified argument count.\n\
	move.l %d1, -(%sp)\n\
	# Call _dl_init (struct link_map *main_map, int argc, char **argv, char **env)\n\
	pea 8(%sp, %d1*4)\n\
	pea 8(%sp)\n\
	move.l %d1, -(%sp)\n\
	move.l ([_rtld_local@GOT.w, %a5]), -(%sp)\n\
	jbsr _dl_init_internal@PLTPC\n\
	addq.l #8, %sp\n\
	addq.l #8, %sp\n\
	| Pass our finalizer function to the user in %a1.\n\
	move.l _dl_fini@GOT.w(%a5), %a1\n\
	| Initialize %fp with the stack pointer.\n\
	move.l %sp, %fp\n\
	| Jump to the user's entry point.\n\
	jmp (%a4)\n\
	.size _dl_start_user, . - _dl_start_user\n\
	.previous");

/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.
   ELF_RTYPE_CLASS_NOCOPY iff TYPE should not be allowed to resolve to one
   of the main executable's symbols, as for a COPY reloc.  */
#define elf_machine_type_class(type) \
  ((((type) == R_68K_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)	\
   | (((type) == R_68K_COPY) * ELF_RTYPE_CLASS_COPY))

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_68K_JMP_SLOT

/* The m68k never uses Elf32_Rel relocations.  */
#define ELF_MACHINE_NO_REL 1

static inline Elf32_Addr elf_machine_fixup_plt(struct link_map *map, lookup_t t,
                const Elf32_Rela *reloc,
                Elf32_Addr *reloc_addr, Elf32_Addr value)
{
	return *reloc_addr = value;
}

/* Return the final value of a plt relocation.  On the m68k the JMP_SLOT
   relocation ignores the addend.  */
static inline Elf32_Addr elf_machine_plt_value(struct link_map *map, const Elf32_Rela *reloc,
                Elf32_Addr value)
{
	return value;
}

#endif /* !dl_machine_h */

#ifdef RESOLVE

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

static inline void elf_machine_rela(struct link_map *map, const Elf32_Rela *reloc,
                                    const Elf32_Sym *sym, const struct r_found_version *version,
                                    Elf32_Addr *const reloc_addr)
{
	const unsigned int r_type = ELF32_R_TYPE(reloc->r_info);

	if (__builtin_expect(r_type == R_68K_RELATIVE, 0))
		*reloc_addr = map->l_addr + reloc->r_addend;

	else {
		const Elf32_Sym *const refsym = sym;
		Elf32_Addr value = RESOLVE(&sym, version, r_type);

		if (sym)
			value += sym->st_value;

		switch (r_type) {
		case R_68K_COPY:
			if (sym == NULL)
				/* This can happen in trace mode if an object could not be
				   found.  */
				break;

			if (sym->st_size > refsym->st_size
			                || (sym->st_size < refsym->st_size && GL(dl_verbose))) {
				const char *strtab;

				strtab = (const void *) D_PTR(map, l_info[DT_STRTAB]);
				_dl_error_printf("\
%s: Symbol `%s' has different size in shared object, consider re-linking\n",
				                 rtld_progname ? : "<program name unknown>",
				                 strtab + refsym->st_name);
			}

			memcpy(reloc_addr, (void *) value, MIN(sym->st_size,
			                                       refsym->st_size));
			break;

		case R_68K_GLOB_DAT:
		case R_68K_JMP_SLOT:
			*reloc_addr = value;
			break;

		case R_68K_8:
			*(char *) reloc_addr = value + reloc->r_addend;
			break;

		case R_68K_16:
			*(short *) reloc_addr = value + reloc->r_addend;
			break;

		case R_68K_32:
			*reloc_addr = value + reloc->r_addend;
			break;

		case R_68K_PC8:
			*(char *) reloc_addr
			        = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
			break;

		case R_68K_PC16:
			*(short *) reloc_addr
			        = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
			break;

		case R_68K_PC32:
			*reloc_addr = value + reloc->r_addend - (Elf32_Addr) reloc_addr;
			break;

		case R_68K_NONE:		/* Alright, Wilbur.  */
			break;

		default:
			_dl_reloc_bad_type(map, r_type, 0);
			break;
		}
	}
}

static inline void elf_machine_rela_relative(Elf32_Addr l_addr, const Elf32_Rela *reloc,
                Elf32_Addr *const reloc_addr)
{
	*reloc_addr = l_addr + reloc->r_addend;
}

static inline void elf_machine_lazy_rel(struct link_map *map,
                                        Elf32_Addr l_addr, const Elf32_Rela *reloc)
{
	Elf32_Addr *const reloc_addr = (void *)(l_addr + reloc->r_offset);

	if (ELF32_R_TYPE(reloc->r_info) == R_68K_JMP_SLOT)
		*reloc_addr += l_addr;

	else
		_dl_reloc_bad_type(map, ELF32_R_TYPE(reloc->r_info), 1);
}

#endif /* RESOLVE */
