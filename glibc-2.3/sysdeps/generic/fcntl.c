/* Copyright (C) 1991, 1995, 1996, 1997, 2002 Free Software Foundation, Inc.
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
#include <fcntl.h>

/* Perform file control operations on FD.  */
int __fcntl(fd, cmd)
int fd;
int cmd;
{
	if (fd < 0) {
		__set_errno(EBADF);
		return -1;
	}

	__set_errno(ENOSYS);
	return -1;
}

libc_hidden_def(__fcntl)
stub_warning(fcntl)

weak_alias(__fcntl, fcntl)
#include <stub-tag.h>
