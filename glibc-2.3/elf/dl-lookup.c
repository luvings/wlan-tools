/* Look up a symbol in the loaded objects.
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

#include <alloca.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ldsodefs.h>
#include "dl-hash.h"
#include <dl-machine.h>
#include <bits/libc-lock.h>
#include <tls.h>

#include <assert.h>

#define VERSTAG(tag)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX (tag))

/* We need this string more than once.  */
static const char undefined_msg[] = "undefined symbol: ";


struct sym_val {
	const ElfW(Sym) *s;
	struct link_map *m;
};


#define make_string(string, rest...) \
  ({									      \
    const char *all[] = { string, ## rest };				      \
    size_t len, cnt;							      \
    char *result, *cp;							      \
									      \
    len = 1;								      \
    for (cnt = 0; cnt < sizeof (all) / sizeof (all[0]); ++cnt)		      \
      len += strlen (all[cnt]);						      \
									      \
    cp = result = alloca (len);						      \
    for (cnt = 0; cnt < sizeof (all) / sizeof (all[0]); ++cnt)		      \
      cp = __stpcpy (cp, all[cnt]);					      \
									      \
    result;								      \
  })

/* Statistics function.  */
#ifdef SHARED
# define bump_num_relocations() ++GL(dl_num_relocations)
#else
# define bump_num_relocations() ((void) 0)
#endif



/* We have two different situations when looking up a simple: with or
   without versioning.  gcc is not able to optimize a single function
   definition serving for both purposes so we define two functions.  */
#define VERSIONED	0
#include "do-lookup.h"

#define VERSIONED	1
#include "do-lookup.h"


/* Add extra dependency on MAP to UNDEF_MAP.  */
static int
internal_function add_dependency(struct link_map *undef_map, struct link_map *map)
{
	struct link_map **list;
	struct link_map *runp;
	unsigned int act;
	unsigned int i;
	int result = 0;

	/* Avoid self-references.  */
	if (undef_map == map)
		return 0;

	/* Don't create cross-reference between modules which are
	   dynamically loaded by the same dlopen() call.  */
	if (undef_map->l_opencount == 0 && map->l_opencount == 0)
		return 0;

	/* Make sure nobody can unload the object while we are at it.  */
	__libc_lock_lock_recursive(GL(dl_load_lock));

	/* Determine whether UNDEF_MAP already has a reference to MAP.  First
	   look in the normal dependencies.  */
	if (undef_map->l_searchlist.r_list != NULL) {
		list = undef_map->l_initfini;

		for (i = 0; list[i] != NULL; ++i)
			if (list[i] == map)
				goto out;
	}

	/* No normal dependency.  See whether we already had to add it
	   to the special list of dynamic dependencies.  */
	list = undef_map->l_reldeps;
	act = undef_map->l_reldepsact;

	for (i = 0; i < act; ++i)
		if (list[i] == map)
			goto out;

	/* The object is not yet in the dependency list.  Before we add
	   it make sure just one more time the object we are about to
	   reference is still available.  There is a brief period in
	   which the object could have been removed since we found the
	   definition.  */
	runp = GL(dl_loaded);

	while (runp != NULL && runp != map)
		runp = runp->l_next;

	if (runp != NULL) {
		/* The object is still available.  Add the reference now.  */
		if (__builtin_expect(act >= undef_map->l_reldepsmax, 0)) {
			/* Allocate more memory for the dependency list.  Since this
			   can never happen during the startup phase we can use
			   `realloc'.  */
			void *newp;

			undef_map->l_reldepsmax += 5;
			newp = realloc(undef_map->l_reldeps,
			               undef_map->l_reldepsmax
			               * sizeof(struct link_map *));

			if (__builtin_expect(newp != NULL, 1))
				undef_map->l_reldeps = (struct link_map **) newp;

			else
				/* Correct the addition.  */
				undef_map->l_reldepsmax -= 5;
		}

		/* If we didn't manage to allocate memory for the list this is
		 no fatal mistake.  We simply increment the use counter of the
		 referenced object and don't record the dependencies.  This
		 means this increment can never be reverted and the object
		 will never be unloaded.  This is semantically the correct
		 behavior.  */
		if (__builtin_expect(act < undef_map->l_reldepsmax, 1))
			undef_map->l_reldeps[undef_map->l_reldepsact++] = map;

		if (map->l_searchlist.r_list != NULL)
			/* And increment the counter in the referenced object.  */
			++map->l_opencount;

		else

			/* We have to bump the counts for all dependencies since so far
			   this object was only a normal or transitive dependency.
			   Now it might be closed with _dl_close() directly.  */
			for (list = map->l_initfini; *list != NULL; ++list)
				++(*list)->l_opencount;

		/* Display information if we are debugging.  */
		if (__builtin_expect(GL(dl_debug_mask) & DL_DEBUG_FILES, 0))
			INTUSE(_dl_debug_printf)("\
\nfile=%s;  needed by %s (relocation dependency)\n\n",
			                         map->l_name[0] ? map->l_name : rtld_progname,
			                         undef_map->l_name[0]
			                         ? undef_map->l_name : rtld_progname);

	} else
		/* Whoa, that was bad luck.  We have to search again.  */
		result = -1;

out:
	/* Release the lock.  */
	__libc_lock_unlock_recursive(GL(dl_load_lock));

	return result;
}

static int
internal_function _dl_do_lookup(const char *undef_name, unsigned long int hash,
                                const ElfW(Sym) *ref, struct sym_val *result,
                                struct r_scope_elem *scope, size_t i, int flags,
                                struct link_map *skip, int type_class);
static int
internal_function _dl_do_lookup_versioned(const char *undef_name, unsigned long int hash,
                const ElfW(Sym) *ref, struct sym_val *result,
                struct r_scope_elem *scope, size_t i,
                const struct r_found_version *const version,
                struct link_map *skip, int type_class);

static void
internal_function _dl_debug_bindings(const char *undef_name, struct link_map *undef_map,
                                     const ElfW(Sym) **ref, struct r_scope_elem *symbol_scope[],
                                     struct sym_val *value,
                                     const struct r_found_version *version, int type_class,
                                     int protected);

/* Search loaded objects' symbol tables for a definition of the symbol
   UNDEF_NAME.  */

lookup_t
internal_function _dl_lookup_symbol(const char *undef_name, struct link_map *undef_map,
                                    const ElfW(Sym) **ref, struct r_scope_elem *symbol_scope[],
                                    int type_class, int flags)
{
	const unsigned long int hash = _dl_elf_hash(undef_name);
	struct sym_val current_value = { NULL, NULL };
	struct r_scope_elem **scope;
	int protected;

	bump_num_relocations();

	/* Search the relevant loaded objects for a definition.  */
	for (scope = symbol_scope; *scope; ++scope)
		if (do_lookup(undef_name, hash, *ref, &current_value, *scope, 0, flags,
		                NULL, type_class))
			break;

	if (__builtin_expect(current_value.s == NULL, 0)) {
		const char *reference_name = undef_map ? undef_map->l_name : NULL;

		if (*ref == NULL || ELFW(ST_BIND)((*ref)->st_info) != STB_WEAK)
			/* We could find no value for a strong reference.  */
			/* XXX We cannot translate the messages.  */
			_dl_signal_cerror(0, (reference_name[0]
			                      ? reference_name
			                      : (rtld_progname ? : "<main program>")),
			                  N_("relocation error"),
			                  make_string(undefined_msg, undef_name));

		*ref = NULL;
		return 0;
	}

	protected = *ref && ELFW(ST_VISIBILITY)((*ref)->st_other) == STV_PROTECTED;

	if (__builtin_expect(protected != 0, 0)) {
		/* It is very tricky.  We need to figure out what value to
		   return for the protected symbol.  */
		struct sym_val protected_value = { NULL, NULL };

		for (scope = symbol_scope; *scope; ++scope)
			if (_dl_do_lookup(undef_name, hash, *ref, &protected_value, *scope,
			                  0, flags, NULL, ELF_RTYPE_CLASS_PLT))
				break;

		if (protected_value.s != NULL && protected_value.m != undef_map) {
			current_value.s = *ref;
			current_value.m = undef_map;
		}
	}

	/* We have to check whether this would bind UNDEF_MAP to an object
	   in the global scope which was dynamically loaded.  In this case
	   we have to prevent the latter from being unloaded unless the
	   UNDEF_MAP object is also unloaded.  */
	if (__builtin_expect(current_value.m->l_type == lt_loaded, 0)
	                /* Don't do this for explicit lookups as opposed to implicit
	                 runtime lookups.  */
	                && (flags & DL_LOOKUP_ADD_DEPENDENCY) != 0
	                /* Add UNDEF_MAP to the dependencies.  */
	                && add_dependency(undef_map, current_value.m) < 0)
		/* Something went wrong.  Perhaps the object we tried to reference
		 was just removed.  Try finding another definition.  */
		return INTUSE(_dl_lookup_symbol)(undef_name, undef_map, ref,
		                                 symbol_scope, type_class, flags);

	if (__builtin_expect(GL(dl_debug_mask)
	                     & (DL_DEBUG_BINDINGS | DL_DEBUG_PRELINK), 0))
		_dl_debug_bindings(undef_name, undef_map, ref, symbol_scope,
		                   &current_value, NULL, type_class, protected);

	*ref = current_value.s;
	return LOOKUP_VALUE(current_value.m);
}

INTDEF(_dl_lookup_symbol)


/* This function is nearly the same as `_dl_lookup_symbol' but it
   skips in the first list all objects until SKIP_MAP is found.  I.e.,
   it only considers objects which were loaded after the described
   object.  If there are more search lists the object described by
   SKIP_MAP is only skipped.  */
lookup_t
internal_function
_dl_lookup_symbol_skip(const char *undef_name,
                       struct link_map *undef_map, const ElfW(Sym) **ref,
                       struct r_scope_elem *symbol_scope[],
                       struct link_map *skip_map)
{
	const unsigned long int hash = _dl_elf_hash(undef_name);
	struct sym_val current_value = { NULL, NULL };
	struct r_scope_elem **scope;
	size_t i;
	int protected;

	bump_num_relocations();

	/* Search the relevant loaded objects for a definition.  */
	scope = symbol_scope;

	for (i = 0; (*scope)->r_list[i] != skip_map; ++i)
		assert(i < (*scope)->r_nlist);

	if (! _dl_do_lookup(undef_name, hash, *ref, &current_value, *scope, i,
	                    DL_LOOKUP_RETURN_NEWEST, skip_map, 0))
		while (*++scope)
			if (_dl_do_lookup(undef_name, hash, *ref, &current_value, *scope, 0,
			                  DL_LOOKUP_RETURN_NEWEST, skip_map, 0))
				break;

	if (__builtin_expect(current_value.s == NULL, 0)) {
		*ref = NULL;
		return 0;
	}

	protected = *ref && ELFW(ST_VISIBILITY)((*ref)->st_other) == STV_PROTECTED;

	if (__builtin_expect(protected != 0, 0)) {
		/* It is very tricky.  We need to figure out what value to
		   return for the protected symbol.  */
		struct sym_val protected_value = { NULL, NULL };

		if (i >= (*scope)->r_nlist
		                || !_dl_do_lookup(undef_name, hash, *ref, &protected_value, *scope,
		                                  i, DL_LOOKUP_RETURN_NEWEST, skip_map,
		                                  ELF_RTYPE_CLASS_PLT))
			while (*++scope)
				if (_dl_do_lookup(undef_name, hash, *ref, &protected_value, *scope,
				                  0, DL_LOOKUP_RETURN_NEWEST, skip_map,
				                  ELF_RTYPE_CLASS_PLT))
					break;

		if (protected_value.s != NULL && protected_value.m != undef_map) {
			current_value.s = *ref;
			current_value.m = undef_map;
		}
	}

	if (__builtin_expect(GL(dl_debug_mask)
	                     & (DL_DEBUG_BINDINGS | DL_DEBUG_PRELINK), 0))
		_dl_debug_bindings(undef_name, undef_map, ref, symbol_scope,
		                   &current_value, NULL, 0, protected);

	*ref = current_value.s;
	return LOOKUP_VALUE(current_value.m);
}


/* This function works like _dl_lookup_symbol but it takes an
   additional arguement with the version number of the requested
   symbol.

   XXX We'll see whether we need this separate function.  */
lookup_t
internal_function _dl_lookup_versioned_symbol(const char *undef_name,
                struct link_map *undef_map, const ElfW(Sym) **ref,
                struct r_scope_elem *symbol_scope[],
                const struct r_found_version *version,
                int type_class, int flags)
{
	const unsigned long int hash = _dl_elf_hash(undef_name);
	struct sym_val current_value = { NULL, NULL };
	struct r_scope_elem **scope;
	int protected;

	bump_num_relocations();

	/* No other flag than DL_LOOKUP_ADD_DEPENDENCY is allowed.  */
	assert(flags == 0 || flags == DL_LOOKUP_ADD_DEPENDENCY);

	/* Search the relevant loaded objects for a definition.  */
	for (scope = symbol_scope; *scope; ++scope) {
		int res = do_lookup_versioned(undef_name, hash, *ref, &current_value,
		                              *scope, 0, version, NULL, type_class);

		if (res > 0)
			break;

		if (__builtin_expect(res, 0) < 0) {
			/* Oh, oh.  The file named in the relocation entry does not
			   contain the needed symbol.  */
			const char *reference_name = undef_map ? undef_map->l_name : NULL;

			/* XXX We cannot translate the message.  */
			_dl_signal_cerror(0, (reference_name[0]
			                      ? reference_name
			                      : (rtld_progname ? : "<main program>")),
			                  N_("relocation error"),
			                  make_string("symbol ", undef_name, ", version ",
			                              version->name,
			                              " not defined in file ",
			                              version->filename,
			                              " with link time reference",
			                              res == -2
			                              ? " (no version symbols)" : ""));
			*ref = NULL;
			return 0;
		}
	}

	if (__builtin_expect(current_value.s == NULL, 0)) {
		if (*ref == NULL || ELFW(ST_BIND)((*ref)->st_info) != STB_WEAK) {
			/* We could find no value for a strong reference.  */
			const char *reference_name = undef_map ? undef_map->l_name : NULL;

			/* XXX We cannot translate the message.  */
			_dl_signal_cerror(0, (reference_name[0]
			                      ? reference_name
			                      : (rtld_progname ? : "<main program>")), NULL,
			                  make_string(undefined_msg, undef_name,
			                              ", version ",
			                              version->name ? : NULL));
		}

		*ref = NULL;
		return 0;
	}

	protected = *ref && ELFW(ST_VISIBILITY)((*ref)->st_other) == STV_PROTECTED;

	if (__builtin_expect(protected != 0, 0)) {
		/* It is very tricky.  We need to figure out what value to
		   return for the protected symbol.  */
		struct sym_val protected_value = { NULL, NULL };

		for (scope = symbol_scope; *scope; ++scope)
			if (_dl_do_lookup_versioned(undef_name, hash, *ref, &protected_value,
			                            *scope, 0, version, NULL,
			                            ELF_RTYPE_CLASS_PLT))
				break;

		if (protected_value.s != NULL && protected_value.m != undef_map) {
			current_value.s = *ref;
			current_value.m = undef_map;
		}
	}

	/* We have to check whether this would bind UNDEF_MAP to an object
	   in the global scope which was dynamically loaded.  In this case
	   we have to prevent the latter from being unloaded unless the
	   UNDEF_MAP object is also unloaded.  */
	if (__builtin_expect(current_value.m->l_type == lt_loaded, 0)
	                /* Don't do this for explicit lookups as opposed to implicit
	                 runtime lookups.  */
	                && flags != 0
	                /* Add UNDEF_MAP to the dependencies.  */
	                && add_dependency(undef_map, current_value.m) < 0)
		/* Something went wrong.  Perhaps the object we tried to reference
		 was just removed.  Try finding another definition.  */
		return INTUSE(_dl_lookup_versioned_symbol)(undef_name, undef_map,
		                ref, symbol_scope,
		                version, type_class, flags);

	if (__builtin_expect(GL(dl_debug_mask)
	                     & (DL_DEBUG_BINDINGS | DL_DEBUG_PRELINK), 0))
		_dl_debug_bindings(undef_name, undef_map, ref, symbol_scope,
		                   &current_value, version, type_class, protected);

	*ref = current_value.s;
	return LOOKUP_VALUE(current_value.m);
}

INTDEF(_dl_lookup_versioned_symbol)


/* Similar to _dl_lookup_symbol_skip but takes an additional argument
   with the version we are looking for.  */
lookup_t
internal_function
_dl_lookup_versioned_symbol_skip(const char *undef_name,
                                 struct link_map *undef_map,
                                 const ElfW(Sym) **ref,
                                 struct r_scope_elem *symbol_scope[],
                                 const struct r_found_version *version,
                                 struct link_map *skip_map)
{
	const char *reference_name = undef_map->l_name;
	const unsigned long int hash = _dl_elf_hash(undef_name);
	struct sym_val current_value = { NULL, NULL };
	struct r_scope_elem **scope;
	size_t i;
	int protected;

	bump_num_relocations();

	/* Search the relevant loaded objects for a definition.  */
	scope = symbol_scope;

	for (i = 0; (*scope)->r_list[i] != skip_map; ++i)
		assert(i < (*scope)->r_nlist);

	if (! _dl_do_lookup_versioned(undef_name, hash, *ref, &current_value,
	                              *scope, i, version, skip_map, 0))
		while (*++scope)
			if (_dl_do_lookup_versioned(undef_name, hash, *ref, &current_value,
			                            *scope, 0, version, skip_map, 0))
				break;

	if (__builtin_expect(current_value.s == NULL, 0)) {
		if (*ref == NULL || ELFW(ST_BIND)((*ref)->st_info) != STB_WEAK) {
			/* We could find no value for a strong reference.  */
			const size_t len = strlen(undef_name);
			char buf[sizeof undefined_msg + len];
			__mempcpy(__mempcpy(buf, undefined_msg, sizeof undefined_msg - 1),
			          undef_name, len + 1);
			/* XXX We cannot translate the messages.  */
			_dl_signal_cerror(0, (reference_name[0]
			                      ? reference_name
			                      : (rtld_progname ? : "<main program>")),
			                  NULL, buf);
		}

		*ref = NULL;
		return 0;
	}

	protected = *ref && ELFW(ST_VISIBILITY)((*ref)->st_other) == STV_PROTECTED;

	if (__builtin_expect(protected != 0, 0)) {
		/* It is very tricky.  We need to figure out what value to
		   return for the protected symbol.  */
		struct sym_val protected_value = { NULL, NULL };

		if (i >= (*scope)->r_nlist
		                || !_dl_do_lookup_versioned(undef_name, hash, *ref,
		                                &protected_value, *scope, i, version,
		                                skip_map, ELF_RTYPE_CLASS_PLT))
			while (*++scope)
				if (_dl_do_lookup_versioned(undef_name, hash, *ref,
				                            &protected_value, *scope, 0, version,
				                            skip_map, ELF_RTYPE_CLASS_PLT))
					break;

		if (protected_value.s != NULL && protected_value.m != undef_map) {
			current_value.s = *ref;
			current_value.m = undef_map;
		}
	}

	if (__builtin_expect(GL(dl_debug_mask)
	                     & (DL_DEBUG_BINDINGS | DL_DEBUG_PRELINK), 0))
		_dl_debug_bindings(undef_name, undef_map, ref, symbol_scope,
		                   &current_value, version, 0, protected);

	*ref = current_value.s;
	return LOOKUP_VALUE(current_value.m);
}


/* Cache the location of MAP's hash table.  */

void
internal_function _dl_setup_hash(struct link_map *map)
{
	Elf_Symndx *hash;
	Elf_Symndx nchain;

	if (!map->l_info[DT_HASH])
		return;

	hash = (void *) D_PTR(map, l_info[DT_HASH]);

	map->l_nbuckets = *hash++;
	nchain = *hash++;
	map->l_buckets = hash;
	hash += map->l_nbuckets;
	map->l_chain = hash;
}


static void
internal_function _dl_debug_bindings(const char *undef_name, struct link_map *undef_map,
                                     const ElfW(Sym) **ref, struct r_scope_elem *symbol_scope[],
                                     struct sym_val *value,
                                     const struct r_found_version *version, int type_class,
                                     int protected)
{
	const char *reference_name = undef_map->l_name;

	if (GL(dl_debug_mask) & DL_DEBUG_BINDINGS) {
		INTUSE(_dl_debug_printf)("binding file %s to %s: %s symbol `%s'",
		                         (reference_name[0]
		                          ? reference_name
		                          : (rtld_progname ? : "<main program>")),
		                         value->m->l_name[0]
		                         ? value->m->l_name : rtld_progname,
		                         protected ? "protected" : "normal",
		                         undef_name);

		if (version)
			_dl_debug_printf_c(" [%s]\n", version->name);

		else
			_dl_debug_printf_c("\n");
	}

#ifdef SHARED

	if (GL(dl_debug_mask) & DL_DEBUG_PRELINK) {
		int conflict = 0;
		struct sym_val val = { NULL, NULL };

		if ((GL(dl_trace_prelink_map) == NULL
		                || GL(dl_trace_prelink_map) == GL(dl_loaded))
		                && undef_map != GL(dl_loaded)) {
			const unsigned long int hash = _dl_elf_hash(undef_name);

			if (version == 0)
				_dl_do_lookup(undef_name, hash, *ref, &val,
				              undef_map->l_local_scope[0], 0, 0, NULL,
				              type_class);

			else
				_dl_do_lookup_versioned(undef_name, hash, *ref, &val,
				                        undef_map->l_local_scope[0], 0, version,
				                        NULL, type_class);

			if (val.s != value->s || val.m != value->m)
				conflict = 1;
		}

#ifdef USE_TLS

		if (value->s
		                && (__builtin_expect(ELFW(ST_TYPE)(value->s->st_info)
		                                     == STT_TLS, 0)))
			type_class = 4;

#endif

		if (conflict
		                || GL(dl_trace_prelink_map) == undef_map
		                || GL(dl_trace_prelink_map) == NULL
		                || type_class == 4) {
			_dl_printf("%s 0x%0*Zx 0x%0*Zx -> 0x%0*Zx 0x%0*Zx ",
			           conflict ? "conflict" : "lookup",
			           (int) sizeof(ElfW(Addr)) * 2, undef_map->l_map_start,
			           (int) sizeof(ElfW(Addr)) * 2,
			           ((ElfW(Addr)) *ref) - undef_map->l_map_start,
			           (int) sizeof(ElfW(Addr)) * 2,
			           (ElfW(Addr))(value->s ? value->m->l_map_start : 0),
			           (int) sizeof(ElfW(Addr)) * 2,
			           (ElfW(Addr))(value->s ? value->s->st_value : 0));

			if (conflict)
				_dl_printf("x 0x%0*Zx 0x%0*Zx ",
				           (int) sizeof(ElfW(Addr)) * 2,
				           (ElfW(Addr))(val.s ? val.m->l_map_start : 0),
				           (int) sizeof(ElfW(Addr)) * 2,
				           (ElfW(Addr))(val.s ? val.s->st_value : 0));

			_dl_printf("/%x %s\n", type_class, undef_name);
		}
	}

#endif
}

/* These are here so that we only inline do_lookup{,_versioned} in the common
   case, not everywhere.  */
static int __attribute_noinline__
internal_function _dl_do_lookup(const char *undef_name, unsigned long int hash,
                                const ElfW(Sym) *ref, struct sym_val *result,
                                struct r_scope_elem *scope, size_t i, int flags,
                                struct link_map *skip, int type_class)
{
	return do_lookup(undef_name, hash, ref, result, scope, i, flags, skip,
	                 type_class);
}

static int __attribute_noinline__
internal_function _dl_do_lookup_versioned(const char *undef_name, unsigned long int hash,
                const ElfW(Sym) *ref, struct sym_val *result,
                struct r_scope_elem *scope, size_t i,
                const struct r_found_version *const version,
                struct link_map *skip, int type_class)
{
	return do_lookup_versioned(undef_name, hash, ref, result, scope, i,
	                           version, skip, type_class);
}
