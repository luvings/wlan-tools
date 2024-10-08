/* Copyright (C) 2001 Free Software Foundation, Inc.
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
#include <sys/socket.h>
#include <sysdep.h>

/* Read N bytes into BUF from socket FD.
   Returns the number read or -1 for errors.  */

ssize_t __libc_recv(int fd, void *buf, size_t n, int flags)
{

	return INLINE_SYSCALL(recvfrom, 6, fd, buf, n, flags, NULL, NULL);
}

weak_alias(__libc_recv, __recv)
weak_alias(__recv, recv)
