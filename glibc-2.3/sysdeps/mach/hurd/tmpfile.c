/* Open a stdio stream on an anonymous temporary file.  Hurd version.
   Copyright (C) 2001,02 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <hurd.h>
#include <hurd/fs.h>
#include <hurd/fd.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef USE_IN_LIBIO
# include <iolibio.h>
# define __fdopen INTUSE(_IO_fdopen)
# define tmpfile __new_tmpfile
#endif

/* This returns a new stream opened on a temporary file (generated
   by tmpnam).  The file is opened with mode "w+b" (binary read/write).
   If we couldn't generate a unique filename or the file couldn't
   be opened, NULL is returned.  */
FILE *tmpfile(void)
{
	error_t err;
	file_t file;
	int fd;
	FILE *f;

	/* Get a port to the directory that will contain the file.  */
	const char *dirname = __secure_getenv("TMPDIR") ? : P_tmpdir;
	file_t dir = __file_name_lookup(dirname, 0, 0);

	if (dir == MACH_PORT_NULL)
		return NULL;

	/* Create an unnamed file in the temporary directory.  */
	err = __dir_mkfile(dir, O_RDWR, S_IRUSR | S_IWUSR, &file);
	__mach_port_deallocate(__mach_task_self(), dir);

	if (err)
		return __hurd_fail(err), NULL;

	/* Get a file descriptor for that port.  POSIX.1 requires that streams
	   returned by tmpfile allocate file descriptors as fopen would.  */
	fd = _hurd_intern_fd(file, O_RDWR, 1);  /* dealloc on error */

	if (fd < 0)
		return NULL;

	/* Open a stream on the unnamed file.
	   It will cease to exist when this stream is closed.  */
	if ((f = __fdopen(fd, "w+b")) == NULL)
		__close(fd);

	return f;
}

#ifdef USE_IN_LIBIO
# undef tmpfile
# include <shlib-compat.h>
versioned_symbol(libc, __new_tmpfile, tmpfile, GLIBC_2_1);
#endif

weak_alias(tmpfile, tmpfile64)
