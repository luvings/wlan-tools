/* Copyright (C) 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2001.

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
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "gai_misc.h"


/* We need this special structure to handle asynchronous I/O.  */
struct async_waitlist {
	int counter;
	struct sigevent sigev;
	struct waitlist list[0];
};


int getaddrinfo_a(int mode, struct gaicb *list[], int ent, struct sigevent *sig)
{
	struct sigevent defsigev;
	struct requestlist *requests[ent];
	int cnt;
	volatile int total = 0;
	int result = 0;

	/* Check arguments.  */
	if (mode != GAI_WAIT && mode != GAI_NOWAIT) {
		__set_errno(EINVAL);
		return EAI_SYSTEM;
	}

	if (sig == NULL) {
		defsigev.sigev_notify = SIGEV_NONE;
		sig = &defsigev;
	}

	/* Request the mutex.  */
	pthread_mutex_lock(&__gai_requests_mutex);

	/* Now we can enqueue all requests.  Since we already acquired the
	   mutex the enqueue function need not do this.  */
	for (cnt = 0; cnt < ent; ++cnt)
		if (list[cnt] != NULL) {
			requests[cnt] = __gai_enqueue_request(list[cnt]);

			if (requests[cnt] != NULL)
				/* Successfully enqueued.  */
				++total;

			else
				/* Signal that we've seen an error.  `errno' and the error code
				   of the gaicb will tell more.  */
				result = EAI_SYSTEM;

		} else
			requests[cnt] = NULL;

	if (total == 0) {
		/* We don't have anything to do except signalling if we work
		 asynchronously.  */

		/* Release the mutex.  We do this before raising a signal since the
		 signal handler might do a `siglongjmp' and then the mutex is
		 locked forever.  */
		pthread_mutex_unlock(&__gai_requests_mutex);

		if (mode == GAI_NOWAIT)
			__gai_notify_only(sig,
			                  sig->sigev_notify == SIGEV_SIGNAL ? getpid() : 0);

		return result;

	} else if (mode == GAI_WAIT) {
		pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
		struct waitlist waitlist[ent];
		int oldstate;

		total = 0;

		for (cnt = 0; cnt < ent; ++cnt)
			if (requests[cnt] != NULL) {
				waitlist[cnt].cond = &cond;
				waitlist[cnt].next = requests[cnt]->waiting;
				waitlist[cnt].counterp = &total;
				waitlist[cnt].sigevp = NULL;
				waitlist[cnt].caller_pid = 0;	/* Not needed.  */
				requests[cnt]->waiting = &waitlist[cnt];
				++total;
			}

		/* Since `pthread_cond_wait'/`pthread_cond_timedwait' are cancelation
		 points we must be careful.  We added entries to the waiting lists
		 which we must remove.  So defer cancelation for now.  */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);

		while (total > 0)
			pthread_cond_wait(&cond, &__gai_requests_mutex);

		/* Now it's time to restore the cancelation state.  */
		pthread_setcancelstate(oldstate, NULL);

		/* Release the conditional variable.  */
		if (pthread_cond_destroy(&cond) != 0)
			/* This must never happen.  */
			abort();

	} else {
		struct async_waitlist *waitlist;

		waitlist = (struct async_waitlist *)
		           malloc(sizeof(struct async_waitlist)
		                  + (ent * sizeof(struct waitlist)));

		if (waitlist == NULL)
			result = EAI_AGAIN;

		else {
			pid_t caller_pid = sig->sigev_notify == SIGEV_SIGNAL ? getpid() : 0;
			total = 0;

			for (cnt = 0; cnt < ent; ++cnt)
				if (requests[cnt] != NULL) {
					waitlist->list[cnt].cond = NULL;
					waitlist->list[cnt].next = requests[cnt]->waiting;
					waitlist->list[cnt].counterp = &waitlist->counter;
					waitlist->list[cnt].sigevp = &waitlist->sigev;
					waitlist->list[cnt].caller_pid = caller_pid;
					requests[cnt]->waiting = &waitlist->list[cnt];
					++total;
				}

			waitlist->counter = total;
			waitlist->sigev = *sig;
		}
	}

	/* Release the mutex.  */
	pthread_mutex_unlock(&__gai_requests_mutex);

	return result;
}
