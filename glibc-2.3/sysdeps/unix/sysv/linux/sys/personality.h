/* Copyright (C) 2002 Free Software Foundation, Inc.
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

/* Taken verbatim from Linux 2.4 (include/linux/personality.h).  */

#ifndef _SYS_PERSONALITY_H
#define _SYS_PERSONALITY_H 1

#include <features.h>

/* Flags for bug emulation.
   These occupy the top three bytes.  */
enum {
	MMAP_PAGE_ZERO = 0x0100000,
	ADDR_LIMIT_32BIT = 0x0800000,
	SHORT_INODE = 0x1000000,
	WHOLE_SECONDS = 0x2000000,
	STICKY_TIMEOUTS = 0x4000000,
};

/* Personality types.

   These go in the low byte.  Avoid using the top bit, it will
   conflict with error returns.  */
enum {
	PER_LINUX = 0x0000,
	PER_LINUX_32BIT = 0x0000 | ADDR_LIMIT_32BIT,
	PER_SVR4 = 0x0001 | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
	PER_SVR3 = 0x0002 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_SCOSVR3 = 0x0003 | STICKY_TIMEOUTS | WHOLE_SECONDS | SHORT_INODE,
	PER_OSR5 = 0x0003 | STICKY_TIMEOUTS | WHOLE_SECONDS,
	PER_WYSEV386 = 0x0004 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_ISCR4 = 0x0005 | STICKY_TIMEOUTS,
	PER_BSD = 0x0006,
	PER_SUNOS = 0x0006 | STICKY_TIMEOUTS,
	PER_XENIX = 0x0007 | STICKY_TIMEOUTS | SHORT_INODE,
	PER_LINUX32 = 0x0008,
	PER_IRIX32 = 0x0009 | STICKY_TIMEOUTS,	/* IRIX5 32-bit */
	PER_IRIXN32 = 0x000a | STICKY_TIMEOUTS,	/* IRIX6 new 32-bit */
	PER_IRIX64 = 0x000b | STICKY_TIMEOUTS,	/* IRIX6 64-bit */
	PER_RISCOS = 0x000c,
	PER_SOLARIS = 0x000d | STICKY_TIMEOUTS,
	PER_UW7 = 0x000e | STICKY_TIMEOUTS | MMAP_PAGE_ZERO,
	PER_HPUX = 0x000f,
	PER_OSF4 = 0x0010,
	PER_MASK = 0x00ff,
};

__BEGIN_DECLS

/* Set different ABIs (personalities).  */
extern int personality(unsigned long int __persona) __THROW;

__END_DECLS

#endif /* sys/personality.h */
