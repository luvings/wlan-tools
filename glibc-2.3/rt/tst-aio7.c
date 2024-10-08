/* Test for AIO POSIX compliance.
   Copyright (C) 2001,02 Free Software Foundation, Inc.
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

#include <aio.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/* We might wait for 3 seconds, so increase timeout to 10 seconds.  */
#define TIMEOUT 10


#define TEST_FUNCTION do_test ()
static int do_test(void)
{
	int result = 0;
	int piped[2];

	/* Make a pipe that we will never write to, so we can block reading it.  */
	if (pipe(piped) < 0) {
		perror("pipe");
		return 1;
	}

	/* Test for aio_cancel() detecting invalid file descriptor.  */
	{
		struct aiocb cb;
		int fd = -1;

		cb.aio_fildes = fd;
		cb.aio_offset = 0;
		cb.aio_buf = NULL;
		cb.aio_nbytes = 0;
		cb.aio_reqprio = 0;
		cb.aio_sigevent.sigev_notify = SIGEV_NONE;

		errno = 0;

		/* Case one: invalid fds that match.  */
		if (aio_cancel(fd, &cb) != -1 || errno != EBADF) {
			if (errno == ENOSYS) {
				puts("no aio support in this configuration");
				return 0;
			}

			puts("aio_cancel( -1, {-1..} ) did not return -1 or errno != EBADF");
			++result;
		}

		cb.aio_fildes = -2;
		errno = 0;

		/* Case two: invalid fds that do not match; just print warning.  */
		if (aio_cancel(fd, &cb) != -1 || errno != EBADF)
			puts("aio_cancel( -1, {-2..} ) did not return -1 or errno != EBADF");
	}

	/* Test for aio_fsync() detecting bad fd, and fd not open for writing.  */
	{
		struct aiocb cb;
		int fd = -1;

		cb.aio_fildes = fd;
		cb.aio_offset = 0;
		cb.aio_buf = NULL;
		cb.aio_nbytes = 0;
		cb.aio_reqprio = 0;
		cb.aio_sigevent.sigev_notify = SIGEV_NONE;

		errno = 0;

		/* Case one: invalid fd.  */
		if (aio_fsync(O_SYNC, &cb) != -1 || errno != EBADF) {
			puts("aio_fsync( op, {-1..} ) did not return -1 or errno != EBADF");
			++result;
		}

		if ((fd = open("/dev/null", O_RDONLY)) < 0)
			error(1, errno, "opening /dev/null");

		cb.aio_fildes = fd;
		errno = 0;

		/* Case two: valid fd but open for read only.  */
		if (aio_fsync(O_SYNC, &cb) != -1 || errno != EBADF) {
			puts("aio_fsync( op, {RO..} ) did not return -1 or errno != EBADF");
			++result;
		}

		close(fd);
	}

	/* Test for aio_suspend() suspending even if completed elements in list.  */
	{
		const int BYTES = 8, ELEMS = 2;
		int i, r, fd;
		char buff[BYTES];
		char name[] = "/tmp/aio7.XXXXXX";
		struct timespec timeout;
		struct aiocb cb0, cb1;
		struct aiocb *list[ELEMS];

		fd = mkstemp(name);

		if (fd < 0)
			error(1, errno, "creating temp file");

		if (unlink(name))
			error(1, errno, "unlinking temp file");

		if (write(fd, "01234567", BYTES) != BYTES)
			error(1, errno, "writing to temp file");

		cb0.aio_fildes = fd;
		cb0.aio_offset = 0;
		cb0.aio_buf = buff;
		cb0.aio_nbytes = BYTES;
		cb0.aio_reqprio = 0;
		cb0.aio_sigevent.sigev_notify = SIGEV_NONE;

		r = aio_read(&cb0);

		if (r != 0)
			error(1, errno, "reading from file");

		while (aio_error(&(cb0)) == EINPROGRESS)
			usleep(10);

		for (i = 0; i < BYTES; i++)
			printf("%c ", buff[i]);

		printf("\n");

		/* At this point, the first read is completed, so start another one on
		   the read half of a pipe on which nothing will be written.  */
		cb1.aio_fildes = piped[0];
		cb1.aio_offset = 0;
		cb1.aio_buf = buff;
		cb1.aio_nbytes = BYTES;
		cb1.aio_reqprio = 0;
		cb1.aio_sigevent.sigev_notify = SIGEV_NONE;

		r = aio_read(&cb1);

		if (r != 0)
			error(1, errno, "reading from file");

		/* Now call aio_suspend() with the two reads.  It should return
		 * immediately according to the POSIX spec.
		 */
		list[0] = &cb0;
		list[1] = &cb1;
		timeout.tv_sec = 3;
		timeout.tv_nsec = 0;
		r = aio_suspend((const struct aiocb * const *) list, ELEMS, &timeout);

		if (r == -1 && errno == EAGAIN) {
			puts("aio_suspend([done,blocked],2,3) suspended thread");
			++result;
		}
	}

	return result;
}

#include "../test-skeleton.c"
