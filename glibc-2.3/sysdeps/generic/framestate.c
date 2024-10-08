/* __frame_state_for unwinder helper function wrapper.
   Copyright (C) 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2001.

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

#include <dlfcn.h>
#include <stdlib.h>
#define __frame_state_for fallback_frame_state_for
#include <unwind-dw2.c>
#undef __frame_state_for

struct frame_state *fallback_frame_state_for(void *, struct frame_state *)
#ifdef _LIBC
attribute_hidden
#endif
;

typedef struct frame_state *(*framesf)(void *pc, struct frame_state *);
struct frame_state *__frame_state_for(void *pc,
                                      struct frame_state *frame_state);

struct frame_state *
__frame_state_for(void *pc, struct frame_state *frame_state)
{
	static framesf frame_state_for;

	if (frame_state_for == NULL) {
		void *handle = __libc_dlopen("libgcc_s.so.1");

		if (handle == NULL
		                || (frame_state_for
		                    = (framesf) __libc_dlsym(handle, "__frame_state_for")) == NULL)
			frame_state_for = fallback_frame_state_for;
	}

	return frame_state_for(pc, frame_state);
}
