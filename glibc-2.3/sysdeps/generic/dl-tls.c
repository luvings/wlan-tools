/* Thread-local storage handling in the ELF dynamic linker.  Generic version.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>

#include <tls.h>

/* We don't need any of this if TLS is not supported.  */
#ifdef USE_TLS

# include <dl-tls.h>
# include <ldsodefs.h>

/* Value used for dtv entries for which the allocation is delayed.  */
# define TLS_DTV_UNALLOCATED	((void *) -1l)


/* Out-of-memory handler.  */
# ifdef SHARED
static void __attribute__((__noreturn__))
oom(void)
{
	_dl_fatal_printf("cannot allocate memory for thread-local data: ABORT\n");
}

# endif



size_t
internal_function _dl_next_tls_modid(void)
{
	size_t result;

	if (__builtin_expect(GL(dl_tls_dtv_gaps), false)) {
		size_t disp = 0;
		struct dtv_slotinfo_list *runp = GL(dl_tls_dtv_slotinfo_list);

		/* Note that this branch will never be executed during program
		 start since there are no gaps at that time.  Therefore it
		 does not matter that the dl_tls_dtv_slotinfo is not allocated
		 yet when the function is called for the first times.  */
		result = GL(dl_tls_static_nelem) + 1;
		/* If the following would not be true we mustn't have assumed
		 there is a gap.  */
		assert(result <= GL(dl_tls_max_dtv_idx));

		do {
			while (result - disp < runp->len) {
				if (runp->slotinfo[result - disp].map == NULL)
					break;

				++result;
				assert(result <= GL(dl_tls_max_dtv_idx) + 1);
			}

			if (result - disp < runp->len)
				break;

			disp += runp->len;
		} while ((runp = runp->next) != NULL);

		if (result >= GL(dl_tls_max_dtv_idx)) {
			/* The new index must indeed be exactly one higher than the
			   previous high.  */
			assert(result == GL(dl_tls_max_dtv_idx));

			/* There is no gap anymore.  */
			GL(dl_tls_dtv_gaps) = false;

			goto nogaps;
		}

	} else {
		/* No gaps, allocate a new entry.  */
nogaps:
		result = ++GL(dl_tls_max_dtv_idx);
	}

	return result;
}


void
internal_function _dl_determine_tlsoffset(void)
{
	struct dtv_slotinfo *slotinfo;
	size_t max_align = __alignof__(void *);
	size_t offset;
	size_t cnt;

	/* The first element of the dtv slot info list is allocated.  */
	assert(GL(dl_tls_dtv_slotinfo_list) != NULL);
	/* There is at this point only one element in the
	   dl_tls_dtv_slotinfo_list list.  */
	assert(GL(dl_tls_dtv_slotinfo_list)->next == NULL);

# if TLS_TCB_AT_TP
	/* We simply start with zero.  */
	offset = 0;

	slotinfo = GL(dl_tls_dtv_slotinfo_list)->slotinfo;

	for (cnt = 1; slotinfo[cnt].map != NULL; ++cnt) {
		assert(cnt < GL(dl_tls_dtv_slotinfo_list)->len);

		max_align = MAX(max_align, slotinfo[cnt].map->l_tls_align);

		/* Compute the offset of the next TLS block.  */
		offset = roundup(offset + slotinfo[cnt].map->l_tls_blocksize,
		                 slotinfo[cnt].map->l_tls_align);

		/* XXX For some architectures we perhaps should store the
		 negative offset.  */
		slotinfo[cnt].map->l_tls_offset = offset;
	}

	/* The thread descriptor (pointed to by the thread pointer) has its
	   own alignment requirement.  Adjust the static TLS size
	   and TLS offsets appropriately.  */
	// XXX How to deal with this.  We cannot simply add zero bytes
	// XXX after the first (closest to the TCB) TLS block since this
	// XXX would invalidate the offsets the linker creates for the LE
	// XXX model.

	GL(dl_tls_static_size) = offset + TLS_TCB_SIZE;
# elif TLS_DTV_AT_TP
	/* The TLS blocks start right after the TCB.  */
	offset = TLS_TCB_SIZE;

	/* The first block starts right after the TCB.  */
	slotinfo = GL(dl_tls_dtv_slotinfo_list)->slotinfo;

	if (slotinfo[1].map != NULL) {
		size_t prev_size;

		offset = roundup(offset, slotinfo[1].map->l_tls_align);
		slotinfo[1].map->l_tls_offset = offset;
		max_align = slotinfo[1].map->l_tls_align;
		prev_size = slotinfo[1].map->l_tls_blocksize;

		for (cnt = 2; slotinfo[cnt].map != NULL; ++cnt) {
			assert(cnt < GL(dl_tls_dtv_slotinfo_list)->len);

			max_align = MAX(max_align, slotinfo[cnt].map->l_tls_align);

			/* Compute the offset of the next TLS block.  */
			offset = roundup(offset + prev_size,
			                 slotinfo[cnt].map->l_tls_align);

			/* XXX For some architectures we perhaps should store the
			   negative offset.  */
			slotinfo[cnt].map->l_tls_offset = offset;

			prev_size = slotinfo[cnt].map->l_tls_blocksize;
		}

		offset += prev_size;
	}

	GL(dl_tls_static_size) = offset;
# else
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif

	/* The alignment requirement for the static TLS block.  */
	GL(dl_tls_static_align) = MAX(TLS_TCB_ALIGN, max_align);
}


static void *
internal_function allocate_dtv(void *result)
{
	dtv_t *dtv;
	size_t dtv_length;

	/* We allocate a few more elements in the dtv than are needed for the
	   initial set of modules.  This should avoid in most cases expansions
	   of the dtv.  */
	dtv_length = GL(dl_tls_max_dtv_idx) + DTV_SURPLUS;
	dtv = (dtv_t *) malloc((dtv_length + 2) * sizeof(dtv_t));

	if (dtv != NULL) {
		/* This is the initial length of the dtv.  */
		dtv[0].counter = dtv_length;
		/* Initialize all of the rest of the dtv (including the
		 generation counter) with zero to indicate nothing there.  */
		memset(dtv + 1, '\0', (dtv_length + 1) * sizeof(dtv_t));

		/* Add the dtv to the thread data structures.  */
		INSTALL_DTV(result, dtv);

	} else
		result = NULL;

	return result;
}


/* Get size and alignment requirements of the static TLS block.  */
void
internal_function _dl_get_tls_static_info(size_t *sizep, size_t *alignp)
{
	*sizep = GL(dl_tls_static_size);
	*alignp = GL(dl_tls_static_align);
}


void *
internal_function _dl_allocate_tls_storage(void)
{
	void *result;

	/* Allocate a correctly aligned chunk of memory.  */
	result = __libc_memalign(GL(dl_tls_static_align), GL(dl_tls_static_size));

	if (__builtin_expect(result != NULL, 0)) {
		/* Allocate the DTV.  */
		void *allocated = result;

# if TLS_TCB_AT_TP
		/* The TCB follows the TLS blocks.  */
		result = (char *) result + GL(dl_tls_static_size) - TLS_TCB_SIZE;
# endif

		result = allocate_dtv(result);

		if (result == NULL)
			free(allocated);
	}

	return result;
}


void *
internal_function _dl_allocate_tls_init(void *result)
{
	dtv_t *dtv = GET_DTV(result);
	struct dtv_slotinfo_list *listp;
	size_t total = 0;

	if (result == NULL)
		/* The memory allocation failed.  */
		return NULL;

	/* We have to look prepare the dtv for all currently loaded
	   modules using TLS.  For those which are dynamically loaded we
	   add the values indicating deferred allocation.  */
	listp = GL(dl_tls_dtv_slotinfo_list);

	while (1) {
		size_t cnt;

		for (cnt = total == 0 ? 1 : 0; cnt < listp->len; ++cnt) {
			struct link_map *map;
			void *dest;

			/* Check for the total number of used slots.  */
			if (total + cnt > GL(dl_tls_max_dtv_idx))
				break;

			map = listp->slotinfo[cnt].map;

			if (map == NULL)
				/* Unused entry.  */
				continue;

			if (map->l_type == lt_loaded) {
				/* For dynamically loaded modules we simply store
				 the value indicating deferred allocation.  */
				dtv[map->l_tls_modid].pointer = TLS_DTV_UNALLOCATED;
				continue;
			}

			assert(map->l_tls_modid == cnt);
			assert(map->l_tls_blocksize >= map->l_tls_initimage_size);
# if TLS_TCB_AT_TP
			assert(map->l_tls_offset >= map->l_tls_blocksize);
			dest = (char *) result - map->l_tls_offset;
# elif TLS_DTV_AT_TP
			dest = (char *) result + map->l_tls_offset;
# else
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif

			/* Copy the initialization image and clear the BSS part.  */
			dtv[map->l_tls_modid].pointer = dest;
			memset(__mempcpy(dest, map->l_tls_initimage,
			                 map->l_tls_initimage_size), '\0',
			       map->l_tls_blocksize - map->l_tls_initimage_size);
		}

		total += cnt;

		if (total >= GL(dl_tls_max_dtv_idx))
			break;

		listp = listp->next;
		assert(listp != NULL);
	}

	return result;
}

void *
internal_function _dl_allocate_tls(void *mem)
{
	return _dl_allocate_tls_init(mem == NULL
	                             ? _dl_allocate_tls_storage()
	                             : allocate_dtv(mem));
}

INTDEF(_dl_allocate_tls)


void
internal_function
_dl_deallocate_tls(void *tcb, bool dealloc_tcb)
{
	dtv_t *dtv = GET_DTV(tcb);

	/* The array starts with dtv[-1].  */
	free(dtv - 1);

	if (dealloc_tcb)
		free(tcb);
}



# ifdef SHARED
/* The __tls_get_addr function has two basic forms which differ in the
   arguments.  The IA-64 form takes two parameters, the module ID and
   offset.  The form used, among others, on IA-32 takes a reference to
   a special structure which contain the same information.  The second
   form seems to be more often used (in the moment) so we default to
   it.  Users of the IA-64 form have to provide adequate definitions
   of the following macros.  */
#  ifndef GET_ADDR_ARGS
#   define GET_ADDR_ARGS tls_index *ti
#  endif
#  ifndef GET_ADDR_MODULE
#   define GET_ADDR_MODULE ti->ti_module
#  endif
#  ifndef GET_ADDR_OFFSET
#   define GET_ADDR_OFFSET ti->ti_offset
#  endif
/* Systems which do not have tls_index also probably have to define
   DONT_USE_TLS_INDEX.  */

#  ifndef __TLS_GET_ADDR
#   define __TLS_GET_ADDR __tls_get_addr
#  endif


/* Return the symbol address given the map of the module it is in and
   the symbol record.  This is used in dl-sym.c.  */
void *
internal_function _dl_tls_symaddr(struct link_map *map, const ElfW(Sym) *ref)
{
#  ifndef DONT_USE_TLS_INDEX
	tls_index tmp = {
		.ti_module = map->l_tls_modid,
		.ti_offset = ref->st_value
	};

	return __TLS_GET_ADDR(&tmp);
#  else
	return __TLS_GET_ADDR(map->l_tls_modid, ref->st_value);
#  endif
}


static void *allocate_and_init(struct link_map *map)
{
	void *newp;

	newp = __libc_memalign(map->l_tls_align, map->l_tls_blocksize);

	if (newp == NULL)
		oom();

	/* Initialize the memory.  */
	memset(__mempcpy(newp, map->l_tls_initimage, map->l_tls_initimage_size),
	       '\0', map->l_tls_blocksize - map->l_tls_initimage_size);

	return newp;
}


/* The generic dynamic and local dynamic model cannot be used in
   statically linked applications.  */
void *__tls_get_addr(GET_ADDR_ARGS)
{
	dtv_t *dtv = THREAD_DTV();
	struct link_map *the_map = NULL;
	void *p;

	if (__builtin_expect(dtv[0].counter != GL(dl_tls_generation), 0)) {
		struct dtv_slotinfo_list *listp;
		size_t idx;

		/* The global dl_tls_dtv_slotinfo array contains for each module
		 index the generation counter current when the entry was
		 created.  This array never shrinks so that all module indices
		 which were valid at some time can be used to access it.
		 Before the first use of a new module index in this function
		 the array was extended appropriately.  Access also does not
		 have to be guarded against modifications of the array.  It is
		 assumed that pointer-size values can be read atomically even
		 in SMP environments.  It is possible that other threads at
		 the same time dynamically load code and therefore add to the
		 slotinfo list.  This is a problem since we must not pick up
		 any information about incomplete work.  The solution to this
		 is to ignore all dtv slots which were created after the one
		 we are currently interested.  We know that dynamic loading
		 for this module is completed and this is the last load
		 operation we know finished.  */
		idx = GET_ADDR_MODULE;
		listp = GL(dl_tls_dtv_slotinfo_list);

		while (idx >= listp->len) {
			idx -= listp->len;
			listp = listp->next;
		}

		if (dtv[0].counter < listp->slotinfo[idx].gen) {
			/* The generation counter for the slot is higher than what
			   the current dtv implements.  We have to update the whole
			   dtv but only those entries with a generation counter <=
			   the one for the entry we need.  */
			size_t new_gen = listp->slotinfo[idx].gen;
			size_t total = 0;

			/* We have to look through the entire dtv slotinfo list.  */
			listp =  GL(dl_tls_dtv_slotinfo_list);

			do {
				size_t cnt;

				for (cnt = total = 0 ? 1 : 0; cnt < listp->len; ++cnt) {
					size_t gen = listp->slotinfo[cnt].gen;
					struct link_map *map;
					size_t modid;

					if (gen > new_gen)
						/* This is a slot for a generation younger than
						   the one we are handling now.  It might be
						   incompletely set up so ignore it.  */
						continue;

					/* If the entry is older than the current dtv layout
					   we know we don't have to handle it.  */
					if (gen <= dtv[0].counter)
						continue;

					/* If there is no map this means the entry is empty.  */
					map = listp->slotinfo[cnt].map;

					if (map == NULL) {
						/* If this modid was used at some point the memory
						 might still be allocated.  */
						if (dtv[total + cnt].pointer != TLS_DTV_UNALLOCATED)
							free(dtv[total + cnt].pointer);

						continue;
					}

					/* Check whether the current dtv array is large enough.  */
					modid = map->l_tls_modid;
					assert(total + cnt == modid);

					if (dtv[-1].counter < modid) {
						/* Reallocate the dtv.  */
						dtv_t *newp;
						size_t newsize = GL(dl_tls_max_dtv_idx) + DTV_SURPLUS;
						size_t oldsize = dtv[-1].counter;

						assert(map->l_tls_modid <= newsize);

						if (dtv == GL(dl_initial_dtv)) {
							/* This is the initial dtv that was allocated
							   during rtld startup using the dl-minimal.c
							   malloc instead of the real malloc.  We can't
							   free it, we have to abandon the old storage.  */

							newp = malloc((2 + newsize) * sizeof(dtv_t));

							if (newp == NULL)
								oom();

							memcpy(newp, &dtv[-1], oldsize * sizeof(dtv_t));

						} else {
							newp = realloc(&dtv[-1],
							               (2 + newsize) * sizeof(dtv_t));

							if (newp == NULL)
								oom();
						}

						newp[0].counter = newsize;

						/* Clear the newly allocated part.  */
						memset(newp + 2 + oldsize, '\0',
						       (newsize - oldsize) * sizeof(dtv_t));

						/* Point dtv to the generation counter.  */
						dtv = &newp[1];

						/* Install this new dtv in the thread data
						 structures.  */
						INSTALL_NEW_DTV(dtv);
					}

					/* If there is currently memory allocate for this
					   dtv entry free it.  */
					/* XXX Ideally we will at some point create a memory
					   pool.  */
					if (dtv[modid].pointer != TLS_DTV_UNALLOCATED)
						/* Note that free is called for NULL is well.  We
						   deallocate even if it is this dtv entry we are
						   supposed to load.  The reason is that we call
						   memalign and not malloc.  */
						free(dtv[modid].pointer);

					/* This module is loaded dynamically- We defer
					   memory allocation.  */
					dtv[modid].pointer = TLS_DTV_UNALLOCATED;

					if (modid == GET_ADDR_MODULE)
						the_map = map;
				}

				total += listp->len;
			} while ((listp = listp->next) != NULL);

			/* This will be the new maximum generation counter.  */
			dtv[0].counter = new_gen;
		}
	}

	p = dtv[GET_ADDR_MODULE].pointer;

	if (__builtin_expect(p == TLS_DTV_UNALLOCATED, 0)) {
		/* The allocation was deferred.  Do it now.  */
		if (the_map == NULL) {
			/* Find the link map for this module.  */
			size_t idx = GET_ADDR_MODULE;
			struct dtv_slotinfo_list *listp = GL(dl_tls_dtv_slotinfo_list);

			while (idx >= listp->len) {
				idx -= listp->len;
				listp = listp->next;
			}

			the_map = listp->slotinfo[idx].map;
		}

		p = dtv[GET_ADDR_MODULE].pointer = allocate_and_init(the_map);
	}

	return (char *) p + GET_ADDR_OFFSET;
}

# endif

#endif	/* use TLS */
