/* Convert between `struct stat' format, and `struct stat64' format.
   Copyright (C) 2000,01,02 Free Software Foundation, Inc.
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
#include <sys/stat.h>

static inline int xstat64_conv(struct stat *buf, const struct stat64 *buf64)
{
	if (sizeof * buf == sizeof * buf64
	                && sizeof buf->st_ino == sizeof buf64->st_ino
	                && sizeof buf->st_size == sizeof buf64->st_size
	                && sizeof buf->st_blocks == sizeof buf64->st_blocks) {
		*buf = *(struct stat *) buf64;
		return 0;
	}

	buf->st_fstype = buf64->st_fstype;
	buf->st_fsid = buf64->st_fsid;
	buf->st_ino = buf64->st_ino;
	buf->st_gen = buf64->st_gen;
	buf->st_rdev = buf64->st_rdev;
	buf->st_mode = buf64->st_mode;
	buf->st_nlink = buf64->st_nlink;
	buf->st_uid = buf64->st_uid;
	buf->st_gid = buf64->st_gid;
	buf->st_size = buf64->st_size;
	buf->st_atime = buf64->st_atime;
	buf->st_atime_usec = buf64->st_atime_usec;
	buf->st_mtime = buf64->st_mtime;
	buf->st_mtime_usec = buf64->st_mtime_usec;
	buf->st_ctime = buf64->st_ctime;
	buf->st_ctime_usec = buf64->st_ctime_usec;
	buf->st_blksize = buf64->st_blksize;
	buf->st_blocks = buf64->st_blocks;
	buf->st_author = buf64->st_author;
	buf->st_flags = buf64->st_flags;

	if ((sizeof buf->st_ino != sizeof buf64->st_ino
	                && buf->st_ino != buf64->st_ino)
	                || (sizeof buf->st_size != sizeof buf64->st_size
	                    && buf->st_size != buf64->st_size)
	                || (sizeof buf->st_blocks != sizeof buf64->st_blocks
	                    && buf->st_blocks != buf64->st_blocks)) {
		__set_errno(EOVERFLOW);
		return -1;
	}

	return 0;
}
