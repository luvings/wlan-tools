/* Copyright (C) 1992, 1997 Free Software Foundation, Inc.
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

#include <sys/types.h>
#include <sysv_termio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Set the foreground process group ID of FD set PGRP_ID.  */
int tcsetpgrp(fd, pgrp_id)
int fd;
pid_t pgrp_id;
{
	return __ioctl(fd, _TIOCSPGRP, &pgrp_id);
}
