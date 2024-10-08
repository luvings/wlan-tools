/* O_*, F_*, FD_* bit values for Linux/HPPA.
   Copyright (C) 1995-1999, 2000 Free Software Foundation, Inc.
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

#ifndef _FCNTL_H
# error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif

#include <sys/types.h>


/* open/fcntl - O_SYNC is only implemented on blocks devices and on files
   located on an ext2 file system */
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002
#define O_ACCMODE	00000003
#define O_APPEND	00000010
#define O_BLKSEEK	00000100 /* HPUX only */
#define O_CREAT		00000400 /* not fcntl */
#define O_TRUNC		00001000 /* not fcntl */
#define O_EXCL		00002000 /* not fcntl */
#define O_ASYNC		00020000
#define O_SYNC		00100000
#define O_NONBLOCK	00200004 /* HPUX has separate NDELAY & NONBLOCK */
#define O_NDELAY	O_NONBLOCK
#define O_NOCTTY	00400000 /* not fcntl */


#ifdef __USE_GNU
# define O_DIRECT	00040000 /* direct disk access hint - currently ignored */
# define O_DIRECTORY	00010000 /* must be a directory */
# define O_NOFOLLOW	00000200 /* don't follow links */
#endif

#ifdef __USE_LARGEFILE64
# define O_LARGEFILE	00004000
#endif

#if defined __USE_POSIX199309 || defined __USE_UNIX98
# define O_DSYNC		01000000 /* HPUX only */
# define O_RSYNC		02000000 /* HPUX only */
#endif

/* Values for the second argument to `fcntl'.  */
#define F_DUPFD		0	/* Duplicate file descriptor.  */
#define F_GETFD		1	/* Get file descriptor flags.  */
#define F_SETFD		2	/* Set file descriptor flags.  */
#define F_GETFL		3	/* Get file status flags.  */
#define F_SETFL		4	/* Set file status flags.  */
#define F_GETLK		5	/* Get record locking info.  */
#define F_SETLK		6	/* Set record locking info (non-blocking).  */
#define F_SETLKW	7	/* Set record locking info (blocking).  */

#define F_GETLK64	8	/* Get record locking info.  */
#define F_SETLK64	9	/* Set record locking info (non-blocking).  */
#define F_SETLKW64	10	/* Set record locking info (blocking).  */

#if defined __USE_BSD || defined __USE_XOPEN2K
# define F_GETOWN	11	/* Get owner of socket (receiver of SIGIO).  */
# define F_SETOWN	12	/* Set owner of socket (receiver of SIGIO).  */
#endif

#ifdef __USE_GNU
# define F_SETSIG	13	/* Set number of signal to be sent.  */
# define F_GETSIG	14	/* Get number of signal to be sent.  */
#endif

#ifdef __USE_GNU
# define F_SETLEASE     1024    /* Set a lease.  */
# define F_GETLEASE     1025    /* Enquire what lease is active.  */
# define F_NOTIFY       1026    /* Request notfications on a directory.  */
#endif

/* for F_[GET|SET]FL */
#define FD_CLOEXEC	1	/* actually anything with low bit set goes */

/* For posix fcntl() and `l_type' field of a `struct flock' for lockf().  */
#define F_RDLCK		1	/* Read lock.  */
#define F_WRLCK		2	/* Write lock.  */
#define F_UNLCK		3	/* Remove lock.  */

/* for old implementation of bsd flock () */
#define F_EXLCK		4	/* or 3 */
#define F_SHLCK		8	/* or 4 */

#ifdef __USE_BSD
/* operations for bsd flock(), also used by the kernel implementation */
# define LOCK_SH	1	/* shared lock */
# define LOCK_EX	2	/* exclusive lock */
# define LOCK_NB	4	/* or'd with one of the above to prevent
				   blocking */
# define LOCK_UN	8	/* remove lock */
#endif

#ifdef __USE_GNU
/* Types of directory notifications that may be requested with F_NOTIFY.  */
# define DN_ACCESS      0x00000001      /* File accessed.  */
# define DN_MODIFY      0x00000002      /* File modified.  */
# define DN_CREATE      0x00000004      /* File created.  */
# define DN_DELETE      0x00000008      /* File removed.  */
# define DN_RENAME      0x00000010      /* File renamed.  */
# define DN_ATTRIB      0x00000020      /* File changed attibutes.  */
# define DN_MULTISHOT   0x80000000      /* Don't remove notifier.  */
#endif

struct flock {
	short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
	short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
#ifndef __USE_FILE_OFFSET64
	__off_t l_start;	/* Offset where the lock begins.  */
	__off_t l_len;	/* Size of the locked area; zero means until EOF.  */
#else
	__off64_t l_start;	/* Offset where the lock begins.  */
	__off64_t l_len;	/* Size of the locked area; zero means until EOF.  */
#endif
	__pid_t l_pid;	/* Process holding the lock.  */
};

#ifdef __USE_LARGEFILE64
struct flock64 {
	short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
	short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
	__off64_t l_start;	/* Offset where the lock begins.  */
	__off64_t l_len;	/* Size of the locked area; zero means until EOF.  */
	__pid_t l_pid;	/* Process holding the lock.  */
};

#endif

/* Define some more compatibility macros to be backward compatible with
   BSD systems which did not managed to hide these kernel macros.  */
#ifdef	__USE_BSD
# define FAPPEND	O_APPEND
# define FFSYNC		O_FSYNC
# define FASYNC		O_ASYNC
# define FNONBLOCK	O_NONBLOCK
# define FNDELAY	O_NDELAY
#endif /* Use BSD.  */

/* Advise to `posix_fadvise'.  */
#ifdef __USE_XOPEN2K
# define POSIX_FADV_NORMAL	0 /* No further special treatment.  */
# define POSIX_FADV_RANDOM	1 /* Expect random page references.  */
# define POSIX_FADV_SEQUENTIAL	2 /* Expect sequential page references.  */
# define POSIX_FADV_WILLNEED	3 /* Will need these pages.  */
# define POSIX_FADV_DONTNEED	4 /* Don't need these pages.  */
# define POSIX_FADV_NOREUSE	5 /* Data will be accessed once.  */
#endif
