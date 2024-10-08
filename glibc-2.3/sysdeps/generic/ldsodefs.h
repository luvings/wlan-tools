/* Run-time dynamic linker data structures for loaded ELF shared objects.
   Copyright (C) 1995-1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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

#ifndef	_LDSODEFS_H
#define	_LDSODEFS_H	1

#include <features.h>

#include <stdbool.h>
#define __need_size_t
#define __need_NULL
#include <stddef.h>
#include <string.h>

#include <elf.h>
#include <dlfcn.h>
#include <fpu_control.h>
#include <sys/mman.h>
#include <link.h>
#include <dl-lookupcfg.h>
#include <bits/libc-lock.h>
#include <hp-timing.h>
#include <tls.h>

__BEGIN_DECLS

/* We use this macro to refer to ELF types independent of the native wordsize.
   `ElfW(TYPE)' is used in place of `Elf32_TYPE' or `Elf64_TYPE'.  */
#define ELFW(type)	_ElfW (ELF, __ELF_NATIVE_CLASS, type)

/* All references to the value of l_info[DT_PLTGOT],
  l_info[DT_STRTAB], l_info[DT_SYMTAB], l_info[DT_RELA],
  l_info[DT_REL], l_info[DT_JMPREL], and l_info[VERSYMIDX (DT_VERSYM)]
  have to be accessed via the D_PTR macro.  The macro is needed since for
  most architectures the entry is already relocated - but for some not
  and we need to relocate at access time.  */
#ifdef DL_RO_DYN_SECTION
# define D_PTR(map,i) (map->i->d_un.d_ptr + map->l_addr)
#else
# define D_PTR(map,i) map->i->d_un.d_ptr
#endif

/* On some platforms more information than just the address of the symbol
   is needed from the lookup functions.  In this case we return the whole
   link map.  */
#ifdef DL_LOOKUP_RETURNS_MAP
typedef struct link_map *lookup_t;
# define LOOKUP_VALUE(map) map
# define LOOKUP_VALUE_ADDRESS(map) (map ? map->l_addr : 0)
#else
typedef ElfW(Addr) lookup_t;
# define LOOKUP_VALUE(map) map->l_addr
# define LOOKUP_VALUE_ADDRESS(address) address
#endif

/* on some architectures a pointer to a function is not just a pointer
   to the actual code of the function but rather an architecture
   specific descriptor. */
#ifndef ELF_FUNCTION_PTR_IS_SPECIAL
# define DL_SYMBOL_ADDRESS(map, ref) \
 (void *) (LOOKUP_VALUE_ADDRESS (map) + ref->st_value)
# define DL_LOOKUP_ADDRESS(addr) ((ElfW(Addr)) (addr))
# define DL_DT_INIT_ADDRESS(map, start) (start)
# define DL_DT_FINI_ADDRESS(map, start) (start)
#endif

/* Unmap a loaded object, called by _dl_close (). */
#ifndef DL_UNMAP_IS_SPECIAL
# define DL_UNMAP(map) \
 __munmap ((void *) (map)->l_map_start,					      \
	   (map)->l_map_end - (map)->l_map_start)
#endif

/* By default we do not need special support to initialize DSOs loaded
   by statically linked binaries.  */
#ifndef DL_STATIC_INIT
# define DL_STATIC_INIT(map)
#endif

/* Reloc type classes as returned by elf_machine_type_class().
   ELF_RTYPE_CLASS_PLT means this reloc should not be satisfied by
   some PLT symbol, ELF_RTYPE_CLASS_COPY means this reloc should not be
   satisfied by any symbol in the executable.  */
#define ELF_RTYPE_CLASS_PLT 1
#define ELF_RTYPE_CLASS_COPY 2

/* ELF uses the PF_x macros to specify the segment permissions, mmap
   uses PROT_xxx.  In most cases the three macros have the values 1, 2,
   and 3 but not in a matching order.  The following macros allows
   converting from the PF_x values to PROT_xxx values.  */
#define PF_TO_PROT \
  ((PROT_READ << (PF_R * 4))						      \
   | (PROT_WRITE << (PF_W * 4))						      \
   | (PROT_EXEC << (PF_X * 4))						      \
   | ((PROT_READ | PROT_WRITE) << ((PF_R | PF_W) * 4))			      \
   | ((PROT_READ | PROT_EXEC) << ((PF_R | PF_X) * 4))			      \
   | ((PROT_WRITE | PROT_EXEC) << (PF_W | PF_X) * 4)			      \
   | ((PROT_READ | PROT_WRITE | PROT_EXEC) << ((PF_R | PF_W | PF_X) * 4)))


/* For the version handling we need an array with only names and their
   hash values.  */
struct r_found_version {
	const char *name;
	ElfW(Word) hash;

	int hidden;
	const char *filename;
};

/* We want to cache information about the searches for shared objects.  */

enum r_dir_status { unknown, nonexisting, existing };

struct r_search_path_elem {
	/* This link is only used in the `all_dirs' member of `r_search_path'.  */
	struct r_search_path_elem *next;

	/* Strings saying where the definition came from.  */
	const char *what;
	const char *where;

	/* Basename for this search path element.  The string must end with
	   a slash character.  */
	const char *dirname;
	size_t dirnamelen;

	enum r_dir_status status[0];
};

struct r_strlenpair {
	const char *str;
	size_t len;
};


/* A data structure for a simple single linked list of strings.  */
struct libname_list {
	const char *name;		/* Name requested (before search).  */
	struct libname_list *next;	/* Link to next name for this object.  */
	int dont_free;		/* Flag whether this element should be freed
				   if the object is not entirely unloaded.  */
};


/* Test whether given NAME matches any of the names of the given object.  */
static __inline int __attribute__((unused))
_dl_name_match_p(const char *__name, struct link_map *__map)
{
	int __found = strcmp(__name, __map->l_name) == 0;
	struct libname_list *__runp = __map->l_libname;

	while (! __found && __runp != NULL)
		if (strcmp(__name, __runp->name) == 0)
			__found = 1;

		else
			__runp = __runp->next;

	return __found;
}

/* Function used as argument for `_dl_receive_error' function.  The
   arguments are the error code, error string, and the objname the
   error occurred in.  */
typedef void (*receiver_fct)(int, const char *, const char *);

/* Internal functions of the run-time dynamic linker.
   These can be accessed if you link again the dynamic linker
   as a shared library, as in `-lld' or `/lib/ld.so' explicitly;
   but are not normally of interest to user programs.

   The `-ldl' library functions in <dlfcn.h> provide a simple
   user interface to run-time dynamic linking.  */


#ifndef SHARED
# define EXTERN extern
# define GL(name) _##name
#else
# define EXTERN
# ifdef IS_IN_rtld
#  define GL(name) _rtld_local._##name
# else
#  define GL(name) _rtld_global._##name
# endif
struct rtld_global {
#endif
/* Don't change the order of the following elements.  'dl_loaded'
   must remain the first element.  Forever.  */

/* And a pointer to the map for the main map.  */
EXTERN struct link_map *_dl_loaded;
/* Number of object in the _dl_loaded list.  */
EXTERN unsigned int _dl_nloaded;
/* Array representing global scope.  */
EXTERN struct r_scope_elem *_dl_global_scope[2];
/* Direct pointer to the searchlist of the main object.  */
EXTERN struct r_scope_elem *_dl_main_searchlist;
/* Copy of the content of `_dl_main_searchlist'.  */
EXTERN struct r_scope_elem _dl_initial_searchlist;
/* This is zero at program start to signal that the global scope map is
   allocated by rtld.  Later it keeps the size of the map.  It might be
   reset if in _dl_close if the last global object is removed.  */
EXTERN size_t _dl_global_scope_alloc;

/* If nonzero the appropriate debug information is printed.  */
EXTERN int _dl_debug_mask;
#define DL_DEBUG_LIBS	    (1 << 0)
#define DL_DEBUG_IMPCALLS   (1 << 1)
#define DL_DEBUG_BINDINGS   (1 << 2)
#define DL_DEBUG_SYMBOLS    (1 << 3)
#define DL_DEBUG_VERSIONS   (1 << 4)
#define DL_DEBUG_RELOC      (1 << 5)
#define DL_DEBUG_FILES      (1 << 6)
#define DL_DEBUG_STATISTICS (1 << 7)
/* These two are used only internally.  */
#define DL_DEBUG_HELP       (1 << 8)
#define DL_DEBUG_PRELINK    (1 << 9)

/* Cached value of `getpagesize ()'.  */
EXTERN size_t _dl_pagesize;

/* During the program run we must not modify the global data of
   loaded shared object simultanously in two threads.  Therefore we
   protect `_dl_open' and `_dl_close' in dl-close.c.

   This must be a recursive lock since the initializer function of
   the loaded object might as well require a call to this function.
   At this time it is not anymore a problem to modify the tables.  */
__libc_lock_define_recursive(EXTERN, _dl_load_lock)

/* OS version.  */
EXTERN unsigned int _dl_osversion;
/* Platform name.  */
EXTERN const char *_dl_platform;
EXTERN size_t _dl_platformlen;

#ifndef MAP_ANON
/* File descriptor referring to the zero-fill device.  */
EXTERN int _dl_zerofd;
#endif

/* CLK_TCK as reported by the kernel.  */
EXTERN int _dl_clktck;

/* If nonzero print warnings messages.  */
EXTERN int _dl_verbose;

/* Do we do lazy relocations?  */
EXTERN int _dl_lazy;

/* Nonzero if runtime lookups should not update the .got/.plt.  */
EXTERN int _dl_bind_not;

/* Nonzero if references should be treated as weak during runtime
   linking.  */
EXTERN int _dl_dynamic_weak;

/* Default floating-point control word.  */
EXTERN fpu_control_t _dl_fpu_control;

/* The object to be initialized first.  */
EXTERN struct link_map *_dl_initfirst;

#if HP_TIMING_AVAIL || HP_SMALL_TIMING_AVAIL
/* Start time on CPU clock.  */
EXTERN hp_timing_t _dl_cpuclock_offset;

/* Overhead of a high-precision timing measurement.  */
EXTERN hp_timing_t _dl_hp_timing_overhead;
#endif

#ifdef USE_TLS
/* Highest dtv index currently needed.  */
EXTERN size_t _dl_tls_max_dtv_idx;
/* Flag signalling whether there are gaps in the module ID allocation.  */
EXTERN bool _dl_tls_dtv_gaps;
/* Information about the dtv slots.  */
EXTERN struct dtv_slotinfo_list {
	size_t len;
	struct dtv_slotinfo_list *next;
	struct dtv_slotinfo {
		size_t gen;
		struct link_map *map;
	} slotinfo[0];
} *_dl_tls_dtv_slotinfo_list;

/* Number of modules in the static TLS block.  */
EXTERN size_t _dl_tls_static_nelem;
/* Size of the static TLS block.  */
EXTERN size_t _dl_tls_static_size;
/* Alignment requirement of the static TLS block.  */
EXTERN size_t _dl_tls_static_align;

/* Number of additional entries in the slotinfo array of each slotinfo
   list element.  A large number makes it almost certain take we never
   have to iterate beyond the first element in the slotinfo list.  */
# define TLS_SLOTINFO_SURPLUS (62)

/* Number of additional slots in the dtv allocated.  */
# define DTV_SURPLUS	(14)

/* Initial dtv of the main thread, not allocated with normal malloc.  */
EXTERN void *_dl_initial_dtv;
/* Generation counter for the dtv.  */
EXTERN size_t _dl_tls_generation;
#endif

/* Name of the shared object to be profiled (if any).  */
EXTERN const char *_dl_profile;
/* Map of shared object to be profiled.  */
EXTERN struct link_map *_dl_profile_map;
/* Filename of the output file.  */
EXTERN const char *_dl_profile_output;
/* Map of shared object to be prelink traced.  */
EXTERN struct link_map *_dl_trace_prelink_map;
/* Name of the object we want to trace the prelinking.  */
EXTERN const char *_dl_trace_prelink;

/* Expected cache ID.  */
EXTERN int _dl_correct_cache_id;

/* Counters for the number of relocations performed.  */
EXTERN unsigned long int _dl_num_relocations;
EXTERN unsigned long int _dl_num_cache_relocations;

/* Mask for hardware capabilities that are available.  */
EXTERN unsigned long int _dl_hwcap;

/* Mask for important hardware capabilities we honour. */
EXTERN unsigned long int _dl_hwcap_mask;

/* Names of shared object for which the RPATH should be ignored.  */
EXTERN const char *_dl_inhibit_rpath;

/* Location of the binary.  */
EXTERN const char *_dl_origin_path;

/* List of search directories.  */
EXTERN struct r_search_path_elem *_dl_all_dirs;
EXTERN struct r_search_path_elem *_dl_init_all_dirs;

/* File descriptor to write debug messages to.  */
EXTERN int _dl_debug_fd;

/* Get architecture specific definitions.  */
#define PROCINFO_DECL
#include <dl-procinfo.c>

/* Structure describing the dynamic linker itself.  */
EXTERN struct link_map _dl_rtld_map;
#ifdef SHARED
};

# define __rtld_global_attribute__
# ifdef IS_IN_rtld
#  ifdef HAVE_VISIBILITY_ATTRIBUTE
#   ifdef HAVE_SDATA_SECTION
#    define __rtld_local_attribute__ \
	    __attribute__ ((visibility ("hidden"), section (".sdata")))
#    undef __rtld_global_attribute__
#    define __rtld_global_attribute__ __attribute__ ((section (".sdata")))
#   else
#    define __rtld_local_attribute__ __attribute__ ((visibility ("hidden")))
#   endif
#  else
#   define __rtld_local_attribute__
#  endif
extern struct rtld_global _rtld_local __rtld_local_attribute__;
# endif
extern struct rtld_global _rtld_global __rtld_global_attribute__;
#endif
#undef EXTERN

/* Parameters passed to the dynamic linker.  */
extern int _dl_argc attribute_hidden;
extern char **_dl_argv;
#ifdef IS_IN_rtld
extern char **_dl_argv_internal attribute_hidden;
# define rtld_progname (INTUSE(_dl_argv)[0])
#else
# define rtld_progname _dl_argv[0]
#endif

/* The array with message we print as a last resort.  */
extern const char _dl_out_of_memory[];
#ifdef IS_IN_rtld
/* XXX #ifdef should go away.  */
extern const char _dl_out_of_memory_internal[] attribute_hidden;
#endif


/* OS-dependent function to open the zero-fill device.  */
extern int _dl_sysdep_open_zero_fill(void);  /* dl-sysdep.c */


/* Write message on the debug file descriptor.  The parameters are
   interpreted as for a `printf' call.  All the lines start with a
   tag showing the PID.  */
extern void _dl_debug_printf(const char *fmt, ...)
__attribute__((__format__(__printf__, 1, 2)));
extern void _dl_debug_printf_internal(const char *fmt, ...)
__attribute__((__format__(__printf__, 1, 2)))
attribute_hidden;

/* Write message on the debug file descriptor.  The parameters are
   interpreted as for a `printf' call.  All the lines buf the first
   start with a tag showing the PID.  */
extern void _dl_debug_printf_c(const char *fmt, ...)
__attribute__((__format__(__printf__, 1, 2)));


/* Write a message on the specified descriptor FD.  The parameters are
   interpreted as for a `printf' call.  */
extern void _dl_dprintf(int fd, const char *fmt, ...)
__attribute__((__format__(__printf__, 2, 3)))
attribute_hidden;

/* Write a message on the specified descriptor standard output.  The
   parameters are interpreted as for a `printf' call.  */
#define _dl_printf(fmt, args...) \
  _dl_dprintf (STDOUT_FILENO, fmt, ##args)

/* Write a message on the specified descriptor standard error.  The
   parameters are interpreted as for a `printf' call.  */
#define _dl_error_printf(fmt, args...) \
  _dl_dprintf (STDERR_FILENO, fmt, ##args)

/* Write a message on the specified descriptor standard error and exit
   the program.  The parameters are interpreted as for a `printf' call.  */
#define _dl_fatal_printf(fmt, args...) \
  do									      \
    {									      \
      _dl_dprintf (STDERR_FILENO, fmt, ##args);				      \
      _exit (127);							      \
    }									      \
  while (1)


/* This function is called by all the internal dynamic linker functions
   when they encounter an error.  ERRCODE is either an `errno' code or
   zero; OBJECT is the name of the problematical shared object, or null if
   it is a general problem; ERRSTRING is a string describing the specific
   problem.  */
extern void _dl_signal_error(int errcode, const char *object,
                             const char *occurred, const char *errstring)
internal_function
__attribute__((__noreturn__));
extern void _dl_signal_error_internal(int errcode, const char *object,
                                      const char *occurred,
                                      const char *errstring)
internal_function __attribute__((__noreturn__)) attribute_hidden;

/* Like _dl_signal_error, but may return when called in the context of
   _dl_receive_error.  */
extern void _dl_signal_cerror(int errcode, const char *object,
                              const char *occation, const char *errstring)
internal_function;

/* Call OPERATE, receiving errors from `dl_signal_cerror'.  Unlike
   `_dl_catch_error' the operation is resumed after the OPERATE
   function returns.
   ARGS is passed as argument to OPERATE.  */
extern void _dl_receive_error(receiver_fct fct, void (*operate)(void *),
                              void *args)
internal_function;


/* Open the shared object NAME and map in its segments.
   LOADER's DT_RPATH is used in searching for NAME.
   If the object is already opened, returns its existing map.
   For preloaded shared objects PRELOADED is set to a non-zero
   value to allow additional security checks.  */
extern struct link_map *_dl_map_object(struct link_map *loader,
                                       const char *name, int preloaded,
                                       int type, int trace_mode, int mode)
internal_function;
extern struct link_map *_dl_map_object_internal(struct link_map *loader,
                const char *name,
                int preloaded,
                int type, int trace_mode,
                int mode)
internal_function attribute_hidden;

/* Call _dl_map_object on the dependencies of MAP, and set up
   MAP->l_searchlist.  PRELOADS points to a vector of NPRELOADS previously
   loaded objects that will be inserted into MAP->l_searchlist after MAP
   but before its dependencies.  */
extern void _dl_map_object_deps(struct link_map *map,
                                struct link_map **preloads,
                                unsigned int npreloads, int trace_mode,
                                int open_mode)
internal_function;
extern void _dl_map_object_deps_internal(struct link_map *map,
                struct link_map **preloads,
                unsigned int npreloads,
                int trace_mode, int open_mode)
internal_function attribute_hidden;

/* Cache the locations of MAP's hash table.  */
extern void _dl_setup_hash(struct link_map *map)
internal_function attribute_hidden;


/* Search loaded objects' symbol tables for a definition of the symbol
   referred to by UNDEF.  *SYM is the symbol table entry containing the
   reference; it is replaced with the defining symbol, and the base load
   address of the defining object is returned.  SYMBOL_SCOPE is a
   null-terminated list of object scopes to search; each object's
   l_searchlist (i.e. the segment of the dependency tree starting at that
   object) is searched in turn.  REFERENCE_NAME should name the object
   containing the reference; it is used in error messages.
   TYPE_CLASS describes the type of symbol we are looking for.  */
extern lookup_t _dl_lookup_symbol(const char *undef,
                                  struct link_map *undef_map,
                                  const ElfW(Sym) **sym,
                                  struct r_scope_elem *symbol_scope[],
                                  int type_class, int flags)
internal_function;
extern lookup_t _dl_lookup_symbol_internal(const char *undef,
                struct link_map *undef_map,
                const ElfW(Sym) **sym,
                struct r_scope_elem *symbolscope[],
                int type_class, int flags)
internal_function attribute_hidden;

enum {
	/* If necessary add dependency between user and provider object.  */
	DL_LOOKUP_ADD_DEPENDENCY = 1,
	/* Return most recent version instead of default version for
	   unversioned lookup.  */
	DL_LOOKUP_RETURN_NEWEST = 2
};

/* Lookup versioned symbol.  */
extern lookup_t _dl_lookup_versioned_symbol(const char *undef,
                struct link_map *undef_map,
                const ElfW(Sym) **sym,
                struct r_scope_elem *symbol_scope[],
                const struct r_found_version *version,
                int type_class, int explicit)
internal_function;
extern lookup_t _dl_lookup_versioned_symbol_internal(const char *undef,
                struct link_map *undef_map,
                const ElfW(Sym) **sym,
                struct r_scope_elem *symbol_scope[],
                const struct r_found_version *version,
                int type_class,
                int explicit)
internal_function attribute_hidden;

/* For handling RTLD_NEXT we must be able to skip shared objects.  */
extern lookup_t _dl_lookup_symbol_skip(const char *undef,
                                       struct link_map *undef_map,
                                       const ElfW(Sym) **sym,
                                       struct r_scope_elem *symbol_scope[],
                                       struct link_map *skip_this)
internal_function;

/* For handling RTLD_NEXT with versioned symbols we must be able to
   skip shared objects.  */
extern lookup_t _dl_lookup_versioned_symbol_skip(const char *undef,
                struct link_map *undef_map,
                const ElfW(Sym) **sym,
                struct r_scope_elem *symbol_scope[],
                const struct r_found_version *version,
                struct link_map *skip_this)
internal_function;

/* Look up symbol NAME in MAP's scope and return its run-time address.  */
extern ElfW(Addr) _dl_symbol_value(struct link_map *map, const char *name)
internal_function;

/* Allocate a `struct link_map' for a new object being loaded,
   and enter it into the _dl_main_map list.  */
extern struct link_map *_dl_new_object(char *realname, const char *libname,
                                       int type, struct link_map *loader)
internal_function attribute_hidden;

/* Relocate the given object (if it hasn't already been).
   SCOPE is passed to _dl_lookup_symbol in symbol lookups.
   If LAZY is nonzero, don't relocate its PLT.  */
extern void _dl_relocate_object(struct link_map *map,
                                struct r_scope_elem *scope[],
                                int lazy, int consider_profiling);
extern void _dl_relocate_object_internal(struct link_map *map,
                struct r_scope_elem *scope[],
                int lazy, int consider_profiling)
attribute_hidden;

/* Call _dl_signal_error with a message about an unhandled reloc type.
   TYPE is the result of ELFW(R_TYPE) (r_info), i.e. an R_<CPU>_* value.
   PLT is nonzero if this was a PLT reloc; it just affects the message.  */
extern void _dl_reloc_bad_type(struct link_map *map,
                               unsigned int type, int plt)
internal_function __attribute__((__noreturn__));

/* Resolve conflicts if prelinking.  */
extern void _dl_resolve_conflicts(struct link_map *l,
                                  ElfW(Rela) *conflict,
                                  ElfW(Rela) *conflictend);

/* Check the version dependencies of all objects available through
   MAP.  If VERBOSE print some more diagnostics.  */
extern int _dl_check_all_versions(struct link_map *map, int verbose,
                                  int trace_mode)
internal_function;

/* Check the version dependencies for MAP.  If VERBOSE print some more
   diagnostics.  */
extern int _dl_check_map_versions(struct link_map *map, int verbose,
                                  int trace_mode)
internal_function;

/* Initialize the object in SCOPE by calling the constructors with
   ARGC, ARGV, and ENV as the parameters.  */
extern void _dl_init(struct link_map *main_map, int argc, char **argv,
                     char **env) internal_function;

/* Call the finalizer functions of all shared objects whose
   initializer functions have completed.  */
extern void _dl_fini(void) internal_function;

/* The dynamic linker calls this function before and having changing
   any shared object mappings.  The `r_state' member of `struct r_debug'
   says what change is taking place.  This function's address is
   the value of the `r_brk' member.  */
extern void _dl_debug_state(void);
extern void _dl_debug_state_internal(void) attribute_hidden;

/* Initialize `struct r_debug' if it has not already been done.  The
   argument is the run-time load address of the dynamic linker, to be put
   in the `r_ldbase' member.  Returns the address of the structure.  */
extern struct r_debug *_dl_debug_initialize(ElfW(Addr) ldbase)
internal_function;

/* Initialize the basic data structure for the search paths.  */
extern void _dl_init_paths(const char *library_path) internal_function;

/* Gather the information needed to install the profiling tables and start
   the timers.  */
extern void _dl_start_profile(struct link_map *map, const char *output_dir)
internal_function;
extern void _dl_start_profile_internal(struct link_map *map,
                                       const char *output_dir)
internal_function attribute_hidden;

/* The actual functions used to keep book on the calls.  */
extern void _dl_mcount(ElfW(Addr) frompc, ElfW(Addr) selfpc);
extern void _dl_mcount_internal(ElfW(Addr) frompc, ElfW(Addr) selfpc)
attribute_hidden;

/* This function is simply a wrapper around the _dl_mcount function
   which does not require a FROMPC parameter since this is the
   calling function.  */
extern void _dl_mcount_wrapper(void *selfpc);

/* Show the members of the auxiliary array passed up from the kernel.  */
extern void _dl_show_auxv(void) internal_function;

/* Return all environment variables starting with `LD_', one after the
   other.  */
extern char *_dl_next_ld_env_entry(char ***position) internal_function;

/* Return an array with the names of the important hardware capabilities.  */
extern const struct r_strlenpair *_dl_important_hwcaps(const char *platform,
                size_t paltform_len,
                size_t *sz,
                size_t *max_capstrlen)
internal_function;

/* Look up NAME in ld.so.cache and return the file name stored there,
   or null if none is found.  */
extern const char *_dl_load_cache_lookup(const char *name)
internal_function;

/* If the system does not support MAP_COPY we cannot leave the file open
   all the time since this would create problems when the file is replaced.
   Therefore we provide this function to close the file and open it again
   once needed.  */
extern void _dl_unload_cache(void);
extern void _dl_unload_cache_internal(void) attribute_hidden;

/* System-dependent function to read a file's whole contents in the
   most convenient manner available.  *SIZEP gets the size of the
   file.  On error MAP_FAILED is returned.  */
extern void *_dl_sysdep_read_whole_file(const char *file, size_t *sizep,
                                        int prot)
internal_function attribute_hidden;

/* System-specific function to do initial startup for the dynamic linker.
   After this, file access calls and getenv must work.  This is responsible
   for setting __libc_enable_secure if we need to be secure (e.g. setuid),
   and for setting _dl_argc and _dl_argv, and then calling _dl_main.  */
extern ElfW(Addr) _dl_sysdep_start(void **start_argptr,
                                   void (*dl_main)(const ElfW(Phdr) *phdr,
                                                   ElfW(Word) phnum,
                                                   ElfW(Addr) *user_entry))
attribute_hidden;

extern void _dl_sysdep_start_cleanup(void)
internal_function attribute_hidden;


/* Determine next available module ID.  */
extern size_t _dl_next_tls_modid(void) internal_function;

/* Calculate offset of the TLS blocks in the static TLS block.  */
extern void _dl_determine_tlsoffset(void) internal_function;

/* Allocate memory for static TLS block (unless MEM is nonzero) and dtv.  */
extern void *_dl_allocate_tls(void *mem) internal_function;

/* Get size and alignment requirements of the static TLS block.  */
extern void _dl_get_tls_static_info(size_t *sizep, size_t *alignp)
internal_function;

/* These are internal entry points to the two halves of _dl_allocate_tls,
   only used within rtld.c itself at startup time.  */
extern void *_dl_allocate_tls_storage(void)
internal_function attribute_hidden;
extern void *_dl_allocate_tls_init(void *)
internal_function attribute_hidden;

/* Deallocate memory allocated with _dl_allocate_tls.  */
extern void _dl_deallocate_tls(void *tcb, bool dealloc_tcb) internal_function;

/* Return the symbol address given the map of the module it is in and
   the symbol record.  */
extern void *_dl_tls_symaddr(struct link_map *map, const ElfW(Sym) *ref)
internal_function;

__END_DECLS

#endif /* ldsodefs.h */
