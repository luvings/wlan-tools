/* Pseudo implementation of waitid.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Zack Weinberg <zack@rabi.phys.columbia.edu>, 1997.

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
#include <sys/wait.h>
#include <sys/types.h>

#include <assert.h>

int waitid(idtype, id, infop, options)
idtype_t idtype;
id_t id;
siginfo_t *infop;
int options;
{
	pid_t pid, child;
	int status;

	switch (idtype) {
	case P_PID:
		if (id <= 0)
			goto invalid;

		pid = (pid_t) id;
		break;

	case P_PGID:
		if (id < 0 || id == 1)
			goto invalid;

		pid = (pid_t) - id;
		break;

	case P_ALL:
		pid = -1;
		break;

	default:
invalid:
		__set_errno(EINVAL);
		return -1;
	}

	/* Technically we're supposed to return EFAULT if infop is bogus,
	   but that would involve mucking with signals, which is
	   too much hassle.  User will have to deal with SIGSEGV/SIGBUS.
	   We just check for a null pointer. */

	if (infop == NULL) {
		__set_errno(EFAULT);
		return -1;
	}

	child = __waitpid(pid, &status, options);

	if (child == -1)
		/* `waitpid' set `errno' for us.  */
		return -1;

	if (child == 0) {
		/* The WHOHANG bit in OPTIONS is set and there are children available
		 but none has a status for us.  The XPG docs do not mention this
		 case so we clear the `siginfo_t' struct and return successfully.  */
		infop->si_signo = 0;
		infop->si_code = 0;
		return 0;
	}

	/* Decode the status field and set infop members... */
	infop->si_signo = SIGCHLD;
	infop->si_pid = child;
	infop->si_errno = 0;

	if (WIFEXITED(status)) {
		infop->si_code = CLD_EXITED;
		infop->si_status = WEXITSTATUS(status);

	} else if (WIFSIGNALED(status)) {
		infop->si_code = WCOREDUMP(status) ? CLD_DUMPED : CLD_KILLED;
		infop->si_status = WTERMSIG(status);

	} else if (WIFSTOPPED(status)) {
		infop->si_code = CLD_STOPPED;
		infop->si_status = WSTOPSIG(status);
	}

#ifdef WIFCONTINUED

	else if (WIFCONTINUED(status)) {
		infop->si_code = CLD_CONTINUED;
		infop->si_status = SIGCONT;
	}

#endif

	else
		/* Can't happen. */
		assert(! "What?");

	return 0;
}
