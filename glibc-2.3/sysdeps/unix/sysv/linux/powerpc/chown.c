/* chown() compatibility.
   Copyright (C) 1998, 2000, 2002 Free Software Foundation, Inc.
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
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/syscall.h>
#include <stdlib.h>

/*
  In Linux 2.1.x the chown functions have been changed.  A new function lchown
  was introduced.  The new chown now follows symlinks - the old chown and the
  new lchown do not follow symlinks.
  This file emulates chown() under the old kernels.
*/

extern int __syscall_chown(const char *__file,
                           uid_t __owner, gid_t __group);

int __chown(const char *file, uid_t owner, gid_t group)
{
	int err;
	int old_errno;
	char link[PATH_MAX + 2];
	char path[2 * PATH_MAX + 4];
	int loopct;
	int filelen;
	static int libc_old_chown = 0 /* -1=old linux, 1=new linux, 0=unknown */;

	if (libc_old_chown == 1)
		return __syscall_chown(file, owner, group);

	old_errno = errno;

#ifdef __NR_lchown

	if (libc_old_chown == 0) {
		err = __syscall_chown(file, owner, group);

		if (err != -1 || errno != ENOSYS) {
			libc_old_chown = 1;
			return err;
		}

		libc_old_chown = -1;
	}

#endif

	err = __readlink(file, link, PATH_MAX + 1);

	if (err == -1) {
		errno = old_errno;
		return __lchown(file, owner, group);
	}

	filelen = strlen(file) + 1;

	if (filelen > sizeof(path)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	memcpy(path, file, filelen);

	/* 'The system has an arbitrary limit...'  In practise, we'll hit
	   ENAMETOOLONG before this, usually.  */
	for (loopct = 0; loopct < 128; loopct++) {
		int linklen;

		if (err >= PATH_MAX + 1) {
			errno = ENAMETOOLONG;
			return -1;
		}

		link[err] = 0;  /* Null-terminate string, just-in-case.  */

		linklen = strlen(link) + 1;

		if (link[0] == '/')
			memcpy(path, link, linklen);

		else {
			filelen = strlen(path);

			while (filelen > 1 && path[filelen - 1] == '/')
				filelen--;

			while (filelen > 0 && path[filelen - 1] != '/')
				filelen--;

			if (filelen + linklen > sizeof(path)) {
				errno = ENAMETOOLONG;
				return -1;
			}

			memcpy(path + filelen, link, linklen);
		}

		err = __readlink(path, link, PATH_MAX + 1);

		if (err == -1) {
			errno = old_errno;
			return __lchown(path, owner, group);
		}
	}

	errno = ELOOP;
	return -1;
}

libc_hidden_def(__chown)

#include <shlib-compat.h>
versioned_symbol(libc, __chown, chown, GLIBC_2_1);
