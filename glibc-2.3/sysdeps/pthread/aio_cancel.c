/* Cancel requests associated with given file descriptor.
   Copyright (C) 1997, 1998, 2000, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1997.

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


/* We use an UGLY hack to prevent gcc from finding us cheating.  The
   implementation of aio_cancel and aio_cancel64 are identical and so
   we want to avoid code duplication by using aliases.  But gcc sees
   the different parameter lists and prints a warning.  We define here
   a function so that aio_cancel64 has no prototype.  */
#ifndef aio_cancel
#define aio_cancel64 XXX
#include <aio.h>
/* And undo the hack.  */
#undef aio_cancel64
#endif

#include <assert.h>
#include <errno.h>

#include "aio_misc.h"


int aio_cancel(fildes, aiocbp)
int fildes;
struct aiocb *aiocbp;
{
	struct requestlist *req = NULL;
	int result = AIO_ALLDONE;

	/* If fildes is invalid, error. */
	if (fcntl(fildes, F_GETFL) < 0) {
		__set_errno(EBADF);
		return -1;
	}

	/* Request the mutex.  */
	pthread_mutex_lock(&__aio_requests_mutex);

	/* We are asked to cancel a specific AIO request.  */
	if (aiocbp != NULL) {
		/* If the AIO request is not for this descriptor it has no value
		 to look for the request block.  */
		if (aiocbp->aio_fildes == fildes) {
			struct requestlist *last = NULL;

			req = __aio_find_req_fd(fildes);

			if (req == NULL) {
not_found:
				pthread_mutex_unlock(&__aio_requests_mutex);
				__set_errno(EINVAL);
				return -1;
			}

			while (req->aiocbp != (aiocb_union *) aiocbp) {
				last = req;
				req = req->next_prio;

				if (req == NULL)
					goto not_found;
			}

			/* Don't remove the entry if a thread is already working on it.  */
			if (req->running == allocated) {
				result = AIO_NOTCANCELED;
				req = NULL;

			} else {
				/* We can remove the entry.  */
				__aio_remove_request(last, req, 0);

				result = AIO_CANCELED;

				req->next_prio = NULL;
			}
		}

	} else {
		/* Find the beginning of the list of all requests for this
		 desriptor.  */
		req = __aio_find_req_fd(fildes);

		/* If any request is worked on by a thread it must be the first.
		 So either we can delete all requests or all but the first.  */
		if (req != NULL) {
			if (req->running == allocated) {
				struct requestlist *old = req;
				req = req->next_prio;
				old->next_prio = NULL;

				result = AIO_NOTCANCELED;

				if (req != NULL)
					__aio_remove_request(old, req, 1);

			} else {
				result = AIO_CANCELED;

				/* We can remove the entry.  */
				__aio_remove_request(NULL, req, 1);
			}
		}
	}

	/* Mark requests as canceled and send signal.  */
	while (req != NULL) {
		struct requestlist *old = req;
		assert(req->running == yes || req->running == queued);
		req->aiocbp->aiocb.__error_code = ECANCELED;
		req->aiocbp->aiocb.__return_value = -1;
		__aio_notify(req);
		req = req->next_prio;
		__aio_free_request(old);
	}

	/* Release the mutex.  */
	pthread_mutex_unlock(&__aio_requests_mutex);

	return result;
}

#ifndef aio_cancel
weak_alias(aio_cancel, aio_cancel64)
#endif
