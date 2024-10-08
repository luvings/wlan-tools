/* Emulate sigstack function using sigaltstack.
   Copyright (C) 1998, 2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

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

#include <signal.h>
#include <stddef.h>
#include <sys/syscall.h>


#ifdef __NR_sigaltstack
int sigstack(ss, oss)
struct sigstack *ss;
struct sigstack *oss;
{
	stack_t sas;
	stack_t *sasp = NULL;
	stack_t osas;
	stack_t *osasp = oss == NULL ? NULL : &osas;
	int result;

	if (ss != NULL) {
		/* We have to convert the information.  */
		sas.ss_sp = ss->ss_sp;
		sas.ss_flags = ss->ss_onstack ? SS_ONSTACK : 0;

		/* For the size of the stack we have no value we can pass to the
		 kernel.  This is why this function should not be used.  We simply
		 assume that all the memory down to address zero (in case the stack
		 grows down) is available.  */
		sas.ss_size = ss->ss_sp - NULL;

		sasp = &sas;
	}

	/* Call the kernel.  */
	result = __sigaltstack(sasp, osasp);

	/* Convert the result, if wanted and possible.  */
	if (result == 0 && oss != NULL) {
		oss->ss_sp = osas.ss_sp;
		oss->ss_onstack = (osas.ss_flags & SS_ONSTACK) != 0;
	}

	return result;
}

link_warning(sigstack, "the `sigstack' function is dangerous.  `sigaltstack' should be used instead.")
#else
# include <sysdeps/generic/sigstack.c>
#endif
