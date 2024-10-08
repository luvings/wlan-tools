/* Copyright (C) 1993, 95, 96, 97, 98, 99 Free Software Foundation, Inc.
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
   02111-1307 USA.

   As a special exception, if you link the code in this file with
   files compiled with a GNU compiler to produce an executable,
   that does not cause the resulting executable to be covered by
   the GNU Lesser General Public License.  This exception does not
   however invalidate any other reasons why the executable file
   might be covered by the GNU Lesser General Public License.
   This exception applies to code released by its copyright holders
   in files containing the exception.  */

#include "libioP.h"
#include "stdio.h"

#undef _IO_getc

int _IO_getc(fp)
FILE *fp;
{
	int result;
	CHECK_FILE(fp, EOF);
	_IO_cleanup_region_start((void (*) __P((void *))) _IO_funlockfile, fp);
	_IO_flockfile(fp);
	result = _IO_getc_unlocked(fp);
	_IO_funlockfile(fp);
	_IO_cleanup_region_end(0);
	return result;
}

#undef getc

#ifdef weak_alias
weak_alias(_IO_getc, getc)
weak_alias(_IO_getc, fgetc)

#ifndef _IO_MTSAFE_IO
#undef getc_unlocked
weak_alias(_IO_getc, getc_unlocked)
weak_alias(_IO_getc, fgetc_unlocked)
#endif
#endif
