/* Copyright (C) 2000 Free Software Foundation, Inc.
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

#include <sysdep.h>
#include <sys/syscall.h>

#include <linux/posix_types.h>
#include "kernel-features.h"

extern int __syscall_fchown(int __fd,
                            __kernel_uid_t __owner, __kernel_gid_t __group);

#ifdef __NR_fchown32
extern int __syscall_fchown32(int __fd,
                              __kernel_uid32_t __owner, __kernel_gid32_t __group);
# if __ASSUME_32BITUIDS == 0
/* This variable is shared with all files that need to check for 32bit
   uids.  */
extern int __libc_missing_32bit_uids;
# endif
#endif /* __NR_fchown32 */

int __fchown(int fd, uid_t owner, gid_t group)
{
#if  __ASSUME_32BITUIDS > 0
	return INLINE_SYSCALL(fchown32, 3, fd, owner, group);
#else
# ifdef __NR_fchown32

	if (__libc_missing_32bit_uids <= 0) {
		int result;
		int saved_errno = errno;

		result = INLINE_SYSCALL(fchown32, 3, fd, owner, group);

		if (result == 0 || errno != ENOSYS)
			return result;

		__set_errno(saved_errno);
		__libc_missing_32bit_uids = 1;
	}

# endif /* __NR_fchown32 */

	if (((owner + 1) > (uid_t)((__kernel_uid_t) -1U))
	                || ((group + 1) > (gid_t)((__kernel_gid_t) -1U))) {
		__set_errno(EINVAL);
		return -1;
	}

	return INLINE_SYSCALL(fchown, 3, fd, owner, group);
#endif
}

weak_alias(__fchown, fchown)
