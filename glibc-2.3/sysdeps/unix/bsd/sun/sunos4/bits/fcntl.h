/* O_*, F_*, FD_* bit values for SunOS 4.
   Copyright (C) 1991, 1992, 1997 Free Software Foundation, Inc.
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

#ifndef	_FCNTL_H
#error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif


/* File access modes for `open' and `fcntl'.  */
#define	O_RDONLY	0	/* Open read-only.  */
#define	O_WRONLY	1	/* Open write-only.  */
#define	O_RDWR		2	/* Open read/write.  */


/* Bits OR'd into the second argument to open.  */
#define	O_CREAT		0x0200	/* Create file if it doesn't exist.  */
#define	O_EXCL		0x0800	/* Fail if file already exists.  */
#define	O_TRUNC		0x0400	/* Truncate file to zero length.  */
#define	O_NOCTTY	0x8000	/* Don't assign a controlling terminal.  */
#if defined __USE_BSD || defined __USE_SVID
#define	O_ASYNC		0x0040	/* Send SIGIO to owner when data is ready.  */
#define	O_FSYNC		0x2000	/* Synchronous writes.  */
#define	O_SYNC		O_FSYNC
#endif

/* File status flags for `open' and `fcntl'.  */
#define	O_APPEND	0x0008	/* Writes append to the file.  */
#define	O_NONBLOCK	0x4000	/* Non-blocking I/O.  */

/* Sun defines O_NDELAY one way for BSD behavior and another for System V
   behavior.  In the GNU C library, you get the BSD behavior unless you
   define _USG_SOURCE without also defining _BSD_SOURCE or _GNU_SOURCE.  */
#ifdef __USE_BSD
#define	O_NDELAY	0x0004
#endif
#if !defined (O_NDELAY) && defined (__USE_SVID)
#define	O_NDELAY	0x1000
#endif

#ifdef __USE_BSD
/* Bits in the file status flags returned by F_GETFL.
   These are all the O_* flags, plus FREAD and FWRITE, which are
   independent bits set by which of O_RDONLY, O_WRONLY, and O_RDWR, was
   given to `open'.  */
#define FREAD		1
#define	FWRITE		2

/* Traditional Unix names the O_* bits.  */
#define FASYNC		O_ASYNC
#define FCREAT		O_CREAT
#define FEXCL		O_EXCL
#define FTRUNC		O_TRUNC
#define FNOCTTY		O_NOCTTY
#define FFSYNC		O_FSYNC
#define FSYNC		O_SYNC
#define FAPPEND		O_APPEND
#define FNONBLOCK	O_NONBLOCK
#define FNONBIO		O_NONBLOCK
#define FNDELAY		0x0004	/* BSD O_NDELAY.  */
#define	FNBIO		0x1000	/* System V O_NDELAY.  */
#endif

/* Mask for file access modes.  This is system-dependent in case
   some system ever wants to define some other flavor of access.  */
#define	O_ACCMODE	(O_RDONLY|O_WRONLY|O_RDWR)

/* Values for the second argument to `fcntl'.  */
#define	F_DUPFD	  	0	/* Duplicate file descriptor.  */
#define	F_GETFD		1	/* Get file descriptor flags.  */
#define	F_SETFD		2	/* Set file descriptor flags.  */
#define	F_GETFL		3	/* Get file status flags.  */
#define	F_SETFL		4	/* Set file status flags.  */
#ifdef __USE_BSD
#define	F_GETOWN	5	/* Get owner (receiver of SIGIO).  */
#define	F_SETOWN	6	/* Set owner (receiver of SIGIO).  */
#endif
#define	F_GETLK		7	/* Get record locking info.  */
#define	F_SETLK		8	/* Set record locking info (non-blocking).  */
#define	F_SETLKW	9	/* Set record locking info (blocking).  */
#ifdef	__USE_BSD
#define	F_RGETLK	10	/* Get remote record locking info.  */
#define	F_RSETLK	11	/* Set remote locking info (non-blocking).  */
#define	F_CNVT		12	/* Convert a fhandle to an open fd.  */
#define	F_RSETLKW	13	/* Set remote locking info (blocking).  */
#endif

/* File descriptor flags used with F_GETFD and F_SETFD.  */
#define	FD_CLOEXEC	1	/* Close on exec.  */


#include <bits/types.h>

/* The structure describing an advisory lock.  This is the type of the third
   argument to `fcntl' for the F_GETLK, F_SETLK, and F_SETLKW requests.  */
struct flock {
	short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
	short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
	__off_t l_start;	/* Offset where the lock begins.  */
	__off_t l_len;	/* Size of the locked area; zero means until EOF.  */
	short int l_pid;	/* Process holding the lock.  */
	short int l_xxx;	/* Reserved for future use.  */
};

#ifdef	__USE_BSD
/* The structure describing a remote advisory lock.  This is the type of the
   third arg to `fcntl' for the F_RGETLK, F_RSETLK, and F_RSETLKW requests.  */
struct eflock {
	short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.  */
	short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
	__off_t l_start;	/* Offset where the lock begins.  */
	__off_t l_len;	/* Size of the locked area; zero means until EOF.  */
	short int l_pid;	/* Process holding the lock.  */
	short int l_xxx;	/* Reserved for future use.  */
	long int l_rpid;	/* Remote process ID wanting this lock.  */
	long int l_rsys;	/* Remote system ID wanting this lock.  */
};

#endif


/* Values for the `l_type' field of a `struct flock'.  */
#define	F_RDLCK	1	/* Read lock.  */
#define	F_WRLCK	2	/* Write lock.  */
#define	F_UNLCK	3	/* Remove lock.  */
