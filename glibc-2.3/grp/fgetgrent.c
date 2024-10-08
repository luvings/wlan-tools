/* Copyright (C) 1991, 1996, 1997, 1999, 2000 Free Software Foundation, Inc.
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
#include <grp.h>
#include <bits/libc-lock.h>
#include <stdio.h>
#include <stdlib.h>


/* We need to protect the dynamic buffer handling.  */
__libc_lock_define_initialized(static, lock);

static char *buffer;

/* Read one entry from the given stream.  */
struct group *
fgetgrent(FILE *stream)
{
	static size_t buffer_size;
	static struct group resbuf;
	fpos_t pos;
	struct group *result;
	int save;

	if (__builtin_expect(fgetpos(stream, &pos), 0) != 0)
		return NULL;

	/* Get lock.  */
	__libc_lock_lock(lock);

	/* Allocate buffer if not yet available.  */
	if (buffer == NULL) {
		buffer_size = NSS_BUFLEN_GROUP;
		buffer = malloc(buffer_size);
	}

	while (buffer != NULL
	                && (__fgetgrent_r(stream, &resbuf, buffer, buffer_size, &result)
	                    == ERANGE)) {
		char *new_buf;
		buffer_size += NSS_BUFLEN_GROUP;
		new_buf = realloc(buffer, buffer_size);

		if (__builtin_expect(new_buf == NULL, 0)) {
			/* We are out of memory.  Free the current buffer so that the
			   process gets a chance for a normal termination.  */
			save = errno;
			free(buffer);
			__set_errno(save);
		}

		buffer = new_buf;

		/* Reset the stream.  */
		if (fsetpos(stream, &pos) != 0)
			buffer = NULL;
	}

	if (buffer == NULL)
		result = NULL;

	/* Release lock.  Preserve error value.  */
	save = errno;
	__libc_lock_unlock(lock);
	__set_errno(save);

	return result;
}


/* Free all resources if necessary.  */
static void __attribute__((unused))
free_mem(void)
{
	free(buffer);
}

text_set_element(__libc_subfreeres, free_mem);
