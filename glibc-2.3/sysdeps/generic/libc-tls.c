/* Initialization code for TLS in statically linked application.
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

#include <errno.h>
#include <ldsodefs.h>
#include <tls.h>
#include <unistd.h>


#ifdef USE_TLS
extern ElfW(Phdr) *_dl_phdr;
extern size_t _dl_phnum;


/* DTV with just one element plus overhead.  */
static dtv_t static_dtv[3];


static struct {
	struct dtv_slotinfo_list si;
	/* The dtv_slotinfo_list data structure does not include the actual
	   informatin since it is defined as an array of size zero.  We
	   define here the necessary entries.  Not that it is not important
	   whether there is padding or not since we will always access the
	   informatin through the 'si' element.  */
	struct dtv_slotinfo info[2 + TLS_SLOTINFO_SURPLUS];
} static_slotinfo;

/* Fake link map for the application.  */
static struct link_map static_map;


/* Additional definitions needed by TLS initialization.  */
#ifdef TLS_INIT_HELPER
TLS_INIT_HELPER
#endif


void
__libc_setup_tls(size_t tcbsize, size_t tcbalign)
{
	void *tlsblock;
	size_t memsz = 0;
	size_t filesz = 0;
	void *initimage = NULL;
	size_t align = 0;
	size_t max_align = tcbalign;
	size_t tcb_offset;
	ElfW(Phdr) *phdr;

	/* Look through the TLS segment if there is any.  */
	if (_dl_phdr != NULL)
		for (phdr = _dl_phdr; phdr < &_dl_phdr[_dl_phnum]; ++phdr)
			if (phdr->p_type == PT_TLS) {
				/* Remember the values we need.  */
				memsz = phdr->p_memsz;
				filesz = phdr->p_filesz;
				initimage = (void *) phdr->p_vaddr;
				align = phdr->p_align;

				if (phdr->p_align > max_align)
					max_align = phdr->p_align;

				break;
			}

	if (memsz == 0 && tcbsize == 0)
		/* We do not need a TLS block and no thread descriptor.  */
		return;

	/* We have to set up the TCB block which also (possibly) contains
	   'errno'.  Therefore we avoid 'malloc' which might touch 'errno'.
	   Instead we use 'sbrk' which would only uses 'errno' if it fails.
	   In this case we are right away out of memory and the user gets
	   what she/he deserves.  */
# if TLS_TCB_AT_TP
	tlsblock = __sbrk(roundup(memsz, tcbalign) + tcbsize + max_align);
# elif TLS_DTV_AT_TP
	tlsblock = __sbrk(roundup(tcbsize, align) + memsz + max_align);
# else
	/* In case a model with a different layout for the TCB and DTV
	   is defined add another #elif here and in the following #ifs.  */
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif

	/* Align the TLS block.  */
	tlsblock = (void *)(((uintptr_t) tlsblock + max_align - 1)
	                    & ~(max_align - 1));

	/* Initialize the dtv.  [0] is the length, [1] the generation counter.  */
	static_dtv[0].counter = 1;
	// static_dtv[1].counter = 0;		would be needed if not already done

	/* Initialize the TLS block.  */
# if TLS_TCB_AT_TP
	static_dtv[2].pointer = tlsblock;
# elif TLS_DTV_AT_TP
	tcb_offset = roundup(tcbsize, align);
	static_dtv[2].pointer = (char *) tlsblock + tcb_offset;
# else
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif
	memset(__mempcpy(static_dtv[2].pointer, initimage, filesz),
	       '\0', memsz - filesz);

	/* Install the pointer to the dtv.  */

	/* Initialize the thread pointer.  */
# if TLS_TCB_AT_TP
	tcb_offset = roundup(memsz, tcbalign);

	INSTALL_DTV((char *) tlsblock + tcb_offset, static_dtv);

	TLS_INIT_TP((char *) tlsblock + tcb_offset, 0);
# elif TLS_DTV_AT_TP
	INSTALL_DTV(tlsblock, static_dtv);
	TLS_INIT_TP(tlsblock, 0);
# else
#  error "Either TLS_TCB_AT_TP or TLS_DTV_AT_TP must be defined"
# endif

	/* We have to create a fake link map which normally would be created
	   by the dynamic linker.  It just has to have enough information to
	   make the TLS routines happy.  */
	static_map.l_tls_align = align;
	static_map.l_tls_blocksize = memsz;
	static_map.l_tls_initimage_size = filesz;
	static_map.l_tls_offset = tcb_offset;
	static_map.l_type = lt_executable;
	static_map.l_tls_modid = 1;

	/* Create the slotinfo list.  */
	static_slotinfo.si.len = 1;	/* Only one element.  */
	// static_slotinfo.si.next = NULL;	already zero

	static_slotinfo.si.slotinfo[1].gen = 0;
	static_slotinfo.si.slotinfo[1].map = &static_map;

	/* The slotinfo list.  Will be extended by the code doing dynamic
	   linking.  */
	GL(dl_tls_max_dtv_idx) = 1;
	GL(dl_tls_dtv_slotinfo_list) = &static_slotinfo.si;

	/* That is the size of the TLS memory for this object.  */
	GL(dl_tls_static_size) = (roundup(memsz, align ? : 1)
# if TLS_TCB_AT_TP
	                          + tcbsize
# endif
	                         );
	/* The alignment requirement for the static TLS block.  */
	GL(dl_tls_static_align) = MAX(TLS_TCB_ALIGN, max_align);
	/* Number of elements in the static TLS block.  */
	GL(dl_tls_static_nelem) = GL(dl_tls_max_dtv_idx);
}


/* This is the minimal initialization function used when libpthread is
   not used.  */
void __attribute__((weak))
__pthread_initialize_minimal(void)
{
	__libc_setup_tls(TLS_INIT_TCB_SIZE, TLS_INIT_TCB_ALIGN);
}

#endif
