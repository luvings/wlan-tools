/* Copyright (C) 1997, 1998, 2000, 2002 Free Software Foundation, Inc.
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
#include <signal.h>
#define __need_NULL
#include <stddef.h>

#include <sysdep.h>
#include <sys/syscall.h>
#include <bp-checks.h>

#ifdef __NR_rt_sigtimedwait
extern int __syscall_rt_sigtimedwait(const sigset_t *__unbounded, siginfo_t *__unbounded,
                                     const struct timespec *__unbounded, size_t);


/* Return any pending signal or wait for one for the given time.  */
int __sigwait(set, sig)
const sigset_t *set;
int *sig;
{
	int ret;

	/* XXX The size argument hopefully will have to be changed to the
	   real size of the user-level sigset_t.  */
	/* XXX INLINE_SYSCALL_NOERROR candiate.  */
	ret =  INLINE_SYSCALL(rt_sigtimedwait, 4, CHECK_SIGSET(set),
	                      NULL, NULL, _NSIG / 8);

	if (ret != -1) {
		*sig = ret;
		ret = 0;

	} else
		ret = errno;

	return ret;
}

weak_alias(__sigwait, sigwait)
#else
# include <sysdeps/posix/sigwait.c>
#endif
