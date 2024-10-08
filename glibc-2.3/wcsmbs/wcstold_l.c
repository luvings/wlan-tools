/* Convert string representing a number to integer value, using given locale.
   Copyright (C) 1997,98,99,2002 Free Software Foundation, Inc.
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

#define __need_wchar_t
#include <math.h>
#include <stddef.h>
#include <locale.h>
#include <wchar.h>

#define USE_IN_EXTENDED_LOCALE_MODEL	1

#ifndef __NO_LONG_DOUBLE_MATH

extern long double ____wcstold_l_internal(const wchar_t *, wchar_t **, int,
                __locale_t);
extern unsigned long long int ____wcstoull_l_internal(const wchar_t *,
                wchar_t **, int, int,
                __locale_t);

# include <wcstold.c>
#else
/* There is no `long double' type, use the `double' implementations.  */
extern double ____wcstod_l_internal(const wchar_t *, wchar_t **, int,
                                    __locale_t);
long double ____wcstold_l_internal(const wchar_t *nptr, wchar_t **endptr, int group,
                                   __locale_t loc)
{
	return ____wcstod_l_internal(nptr, endptr, group, loc);
}

long double __wcstold_l(const wchar_t *nptr, wchar_t **endptr, __locale_t loc)
{
	return ____wcstod_l_internal(nptr, endptr, 0, loc);
}

#endif

weak_alias(__wcstold_l, wcstold_l)
