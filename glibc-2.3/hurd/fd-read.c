/* Copyright (C) 1993,94,95,97,99,2002 Free Software Foundation, Inc.
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
#include <unistd.h>
#include <hurd.h>
#include <hurd/fd.h>
#include <string.h>

error_t _hurd_fd_read(struct hurd_fd *fd, void *buf, size_t *nbytes, loff_t offset)
{
	error_t err;
	char *data;
	mach_msg_type_number_t nread;

	error_t readfd(io_t port) {
		return __io_read(port, &data, &nread, offset, *nbytes);
	}

	data = buf;

	if (err = HURD_FD_PORT_USE(fd, _hurd_ctty_input(port, ctty, readfd)))
		return err;

	if (data != buf) {
		memcpy(buf, data, nread);
		__vm_deallocate(__mach_task_self(), (vm_address_t) data, nread);
	}

	*nbytes = nread;
	return 0;
}
