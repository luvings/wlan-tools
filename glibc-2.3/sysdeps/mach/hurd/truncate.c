/* Copyright (C) 1991, 92, 93, 94, 95, 97, 98 Free Software Foundation, Inc.
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
#include <sys/types.h>
#include <unistd.h>
#include <hurd.h>

/* Truncate FILE_NAME to LENGTH bytes.  */
int truncate(file_name, length)
const char *file_name;
off_t length;
{
	error_t err;
	file_t file = __file_name_lookup(file_name, O_WRITE, 0);

	if (file == MACH_PORT_NULL)
		return -1;

	err = __file_set_size(file, length);
	__mach_port_deallocate(__mach_task_self(), file);

	if (err)
		return __hurd_fail(err);

	return 0;
}
