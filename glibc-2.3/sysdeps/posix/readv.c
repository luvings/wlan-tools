/* Copyright (C) 1991, 1992, 1996, 1997, 2002 Free Software Foundation, Inc.
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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/param.h>
#include <sys/uio.h>

/* Read data from file descriptor FD, and put the result in the
   buffers described by VECTOR, which is a vector of COUNT `struct iovec's.
   The buffers are filled in the order specified.
   Operates just like `read' (see <unistd.h>) except that data are
   put in VECTOR instead of a contiguous buffer.  */
ssize_t __readv(int fd, const struct iovec *vector, int count)
{
	char *buffer;
	char *buffer_start;
	size_t bytes;
	ssize_t bytes_read;
	int i;
	bool use_malloc = false;

	/* Find the total number of bytes to be read.  */
	bytes = 0;

	for (i = 0; i < count; ++i) {
		/* Check for ssize_t overflow.  */
		if (SSIZE_MAX - bytes < vector[i].iov_len) {
			errno = EINVAL;
			return -1;
		}

		bytes += vector[i].iov_len;
	}

	/* Allocate a temporary buffer to hold the data.  We should normally
	   use alloca since it's faster and does not require synchronization
	   with other threads.  But we cannot if the amount of memory
	   required is too large.  Use 512k as the limit.  */
	if (bytes < 512 * 1024)
		buffer = (char *) __alloca(bytes);

	else {
		buffer = (char *) malloc(bytes);

		if (buffer == NULL)
			/* XXX I don't know whether it is acceptable to try reading
			   the data in chunks.  Probably not so we just fail here.  */
			return -1;

		use_malloc = true;
	}

	/* Read the data.  */
	bytes_read = __read(fd, buffer, bytes);

	if (bytes_read <= 0)
		return -1;

	/* Copy the data from BUFFER into the memory specified by VECTOR.  */
	bytes = bytes_read;
	buffer_start = buffer;

	for (i = 0; i < count; ++i) {
		size_t copy = MIN(vector[i].iov_len, bytes);

		(void) memcpy((void *) vector[i].iov_base, (void *) buffer, copy);

		buffer += copy;
		bytes -= copy;

		if (bytes == 0)
			break;
	}

	if (use_malloc)
		free(buffer_start);

	return bytes_read;
}

#ifndef __readv
weak_alias(__readv, readv)
#endif
