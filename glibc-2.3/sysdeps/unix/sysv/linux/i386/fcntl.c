/* Copyright (C) 2000, 2002 Free Software Foundation, Inc.
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

#include <sysdep.h>
#include <sys/syscall.h>
#include "../kernel-features.h"

extern int __syscall_fcntl(int __fd, int __cmd, ...);
#ifdef __NR_fcntl64
extern int __syscall_fcntl64(int __fd, int __cmd, ...);
#endif

#if __ASSUME_FCNTL64 == 0
/* This variable is shared with all files that check for fcntl64.  */
int __have_no_fcntl64;
#endif

int __libc_fcntl(int fd, int cmd, ...)
{
	va_list ap;
	void *arg;

	va_start(ap, cmd);
	arg = va_arg(ap, void *);
	va_end(ap);

#if __ASSUME_FCNTL64 > 0
	return INLINE_SYSCALL(fcntl64, 3, fd, cmd, arg);
#else
# ifdef __NR_fcntl64

	if (! __have_no_fcntl64) {
		int result = INLINE_SYSCALL(fcntl64, 3, fd, cmd, arg);

		if (result >= 0 || errno != ENOSYS)
			return result;

		__have_no_fcntl64 = 1;
	}

# endif

	switch (cmd) {
	case F_GETLK64:
		/* Convert arg from flock64 to flock and back.  */
	{
		struct flock fl;
		struct flock64 *fl64 = arg;
		int res;

		fl.l_start = (off_t)fl64->l_start;

		/* Check if we can represent the values with the smaller type.  */
		if ((off64_t) fl.l_start != fl64->l_start) {
			__set_errno(EOVERFLOW);
			return -1;
		}

		fl.l_len = (off_t) fl64->l_len;

		/* Check if we can represent the values with the smaller type.  */
		if ((off64_t) fl.l_len != fl64->l_len) {
			__set_errno(EOVERFLOW);
			return -1;
		}

		fl.l_type = fl64->l_type;
		fl.l_whence = fl64->l_whence;
		fl.l_pid = fl64->l_pid;

		res = INLINE_SYSCALL(fcntl, 3, fd, F_GETLK, &fl);

		if (res  != 0)
			return res;

		/* Everything ok, convert back.  */
		fl64->l_type = fl.l_type;
		fl64->l_whence = fl.l_whence;
		fl64->l_start = fl.l_start;
		fl64->l_len = fl.l_len;
		fl64->l_pid = fl.l_pid;

		return 0;
	}

	case F_SETLK64:
	case F_SETLKW64:
		/* Try to convert arg from flock64 to flock.  */
	{
		struct flock fl;
		struct flock64 *fl64 = arg;

		fl.l_start = (off_t) fl64->l_start;

		/* Check if we can represent the values with the smaller type.  */
		if ((off64_t) fl.l_start != fl64->l_start) {
			__set_errno(EOVERFLOW);
			return -1;
		}

		fl.l_len = (off_t)fl64->l_len;

		/* Check if we can represent the values with the smaller type.  */
		if ((off64_t) fl.l_len != fl64->l_len) {
			__set_errno(EOVERFLOW);
			return -1;
		}

		fl.l_type = fl64->l_type;
		fl.l_whence = fl64->l_whence;
		fl.l_pid = fl64->l_pid;
		assert(F_SETLK - F_SETLKW == F_SETLK64 - F_SETLKW64);
		return INLINE_SYSCALL(fcntl, 3, fd, cmd + F_SETLK - F_SETLK64, &fl);
	}

	default:
		return INLINE_SYSCALL(fcntl, 3, fd, cmd, arg);
	}

	return -1;
#endif  /* __ASSUME_FCNTL64  */
}

libc_hidden_def(__libc_fcntl)

weak_alias(__libc_fcntl, __fcntl)
libc_hidden_weak(__fcntl)
weak_alias(__libc_fcntl, fcntl)
