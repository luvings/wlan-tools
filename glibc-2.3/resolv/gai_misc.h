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

#ifndef _GAI_MISC_H
#define _GAI_MISC_H	1

#include <netdb.h>
#include <signal.h>


/* Used to synchronize.  */
struct waitlist {
	struct waitlist *next;

	pthread_cond_t *cond;
	volatile int *counterp;
	/* The next field is used in asynchronous `lio_listio' operations.  */
	struct sigevent *sigevp;
	/* XXX See requestlist, it's used to work around the broken signal
	   handling in Linux.  */
	pid_t caller_pid;
};


/* Used to queue requests..  */
struct requestlist {
	int running;

	struct requestlist *next;

	/* Pointer to the actual data.  */
	struct gaicb *gaicbp;

	/* List of waiting processes.  */
	struct waitlist *waiting;
};

/* To customize the implementation one can use the following struct.
   This implementation follows the one in Irix.  */
struct gaiinit {
	int gai_threads;		/* Maximal number of threads.  */
	int gai_num;		/* Number of expected simultanious requests. */
	int gai_locks;		/* Not used.  */
	int gai_usedba;		/* Not used.  */
	int gai_debug;		/* Not used.  */
	int gai_numusers;		/* Not used.  */
	int gai_idle_time;		/* Number of seconds before idle thread
				   terminates.  */
	int gai_reserved;
};


/* Lock for global I/O list of requests.  */
extern pthread_mutex_t __gai_requests_mutex;


/* Enqueue request.  */
extern struct requestlist *__gai_enqueue_request(struct gaicb *gaicbp)
internal_function;

/* Find request on wait list.  */
extern struct requestlist *__gai_find_request(const struct gaicb *gaicbp)
internal_function;

/* Remove request from waitlist.  */
extern int __gai_remove_request(struct gaicb *gaicbp)
internal_function;

/* Notify initiator of request and tell this everybody listening.  */
extern void __gai_notify(struct requestlist *req)
internal_function;

/* Notify initiator of request.  */
extern int __gai_notify_only(struct sigevent *sigev, pid_t caller_pid)
internal_function;

/* Send the signal.  */
extern int __gai_sigqueue(int sig, const union sigval val, pid_t caller_pid)
internal_function;

#endif /* gai_misc.h */
