/* Copyright (C) 1991,92,94-98,2000,02 Free Software Foundation, Inc.
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
#include <stddef.h>		/* For NULL.  */

#include <sigset-cvt-mask.h>

/* Set the mask of blocked signals to MASK,
   wait for a signal to arrive, and then restore the mask.  */
int __sigpause(int sig_or_mask, int is_sig)
{
	sigset_t set;

	if (is_sig != 0) {
		/* The modern X/Open implementation is requested.  */
		if (__sigprocmask(0, NULL, &set) < 0
		                /* Yes, we call `sigdelset' and not `__sigdelset'.  */
		                || __sigdelset(&set, sig_or_mask) < 0)
			return -1;

	} else if (sigset_set_old_mask(&set, sig_or_mask) < 0)
		return -1;

	return __sigsuspend(&set);
}

libc_hidden_def(__sigpause)

/* We have to provide a default version of this function since the
   standards demand it.  The version which is a bit more reasonable is
   the BSD version.  So make this the default.  */
int
__default_sigpause(int mask)
{
	return __sigpause(mask, 0);
}

#undef sigpause
weak_alias(__default_sigpause, sigpause)


/* We have to provide a default version of this function since the
   standards demand it.  The version which is a bit more reasonable is
   the BSD version.  So make this the default.  */
int
__xpg_sigpause(int sig)
{
	return __sigpause(sig, 1);
}
