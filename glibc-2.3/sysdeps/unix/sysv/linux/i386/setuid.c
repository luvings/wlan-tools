/* Copyright (C) 1998, 2000 Free Software Foundation, Inc.
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
#include <sys/types.h>

#include <sysdep.h>
#include <sys/syscall.h>

#include <linux/posix_types.h>
#include "kernel-features.h"

extern int __syscall_setuid(__kernel_uid_t);

#ifdef __NR_setuid32
extern int __syscall_setuid32(__kernel_uid32_t);
# if __ASSUME_32BITUIDS == 0
/* This variable is shared with all files that need to check for 32bit
   uids.  */
extern int __libc_missing_32bit_uids;
# endif
#endif /* __NR_setuid32 */

int __setuid(uid_t uid)
{
#if __ASSUME_32BITUIDS > 0
	return INLINE_SYSCALL(setuid32, 1, uid);
#else
# ifdef __NR_setuid32

	if (__libc_missing_32bit_uids <= 0) {
		int result;
		int saved_errno = errno;

		result = INLINE_SYSCALL(setuid32, 1, uid);

		if (result == 0 || errno != ENOSYS)
			return result;

		__set_errno(saved_errno);
		__libc_missing_32bit_uids = 1;
	}

# endif /* __NR_setuid32 */

	if (uid == (uid_t) ~0
	                || uid != (uid_t)((__kernel_uid_t) uid)) {
		__set_errno(EINVAL);
		return -1;
	}

	return INLINE_SYSCALL(setuid, 1, uid);
#endif
}

weak_alias(__setuid, setuid)
