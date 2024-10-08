/* lockfile - Handle locking and unlocking of stream.  Singlethreaded version.
   Copyright (C) 1996,97,2000 Free Software Foundation, Inc.
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

#include <stdio.h>

#undef _IO_flockfile
#undef _IO_funlockfile
#undef _IO_ftrylockfile

void __flockfile(FILE *stream)
{
	/* Do nothing.  Using this version does not do any locking.  */
}

weak_alias(__flockfile, flockfile);
#ifdef USE_IN_LIBIO
weak_alias(__flockfile, _IO_flockfile)
#endif


void
__funlockfile(FILE *stream)
{
	/* Do nothing.  Using this version does not do any locking.  */
}

#ifdef USE_IN_LIBIO
weak_alias(__funlockfile, _IO_funlockfile)
#endif
weak_alias(__funlockfile, funlockfile);


int __ftrylockfile(FILE *stream)
{
	/* Do nothing.  Using this version does not do any locking.  */
	return 1;
}

weak_alias(__ftrylockfile, ftrylockfile);
#ifdef USE_IN_LIBIO
weak_alias(__ftrylockfile, _IO_ftrylockfile)
#endif
