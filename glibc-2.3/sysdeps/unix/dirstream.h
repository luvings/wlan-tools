/* Copyright (C) 1993, 1995, 1996 Free Software Foundation, Inc.
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

#ifndef	_DIRSTREAM_H

#define	_DIRSTREAM_H	1

#include <sys/types.h>

#include <bits/libc-lock.h>

/* Directory stream type.

   The miscellaneous Unix `readdir' implementations read directory data
   into a buffer and return `struct dirent *' pointers into it.  */

struct __dirstream {
	int fd;			/* File descriptor.  */

	char *data;			/* Directory block.  */
	size_t allocation;		/* Space allocated for the block.  */
	size_t size;		/* Total valid data in the block.  */
	size_t offset;		/* Current offset into the block.  */

	off_t filepos;		/* Position of next entry to read.  */

	__libc_lock_define(, lock)  /* Mutex lock for this structure.  */
};

#define _DIR_dirfd(dirp)	((dirp)->fd)

#endif	/* dirstream.h */
