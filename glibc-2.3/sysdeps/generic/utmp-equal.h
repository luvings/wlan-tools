/* Helper function for utmp functions to see if two entries are equal.
   Copyright (C) 1996,97,98,99,2000,01,02 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>
   and Paul Janzen <pcj@primenet.com>, 1996.

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

#include <string.h>
#include <utmp.h>

#include "utmp-private.h"

/* Test whether two entries match.  */
static int __utmp_equal(const struct utmp *entry, const struct utmp *match)
{
	return
	        (
#if _HAVE_UT_TYPE - 0
	                (entry->ut_type == INIT_PROCESS
	                 || entry->ut_type == LOGIN_PROCESS
	                 || entry->ut_type == USER_PROCESS
	                 || entry->ut_type == DEAD_PROCESS)
	                &&
	                (match->ut_type == INIT_PROCESS
	                 || match->ut_type == LOGIN_PROCESS
	                 || match->ut_type == USER_PROCESS
	                 || match->ut_type == DEAD_PROCESS)
	                &&
#endif
#if _HAVE_UT_ID - 0
	                (entry->ut_id[0] && match->ut_id[0]
	                 ? strncmp(entry->ut_id, match->ut_id, sizeof match->ut_id) == 0
	                 : strncmp(entry->ut_line, match->ut_line, sizeof match->ut_line) == 0)
#else
	                strncmp(entry->ut_line, match->ut_line, sizeof match->ut_line) == 0
#endif
	        );
}
