/* Implementation of the dgettext(3) function.
   Copyright (C) 1995-1997, 2000, 2001, 2002 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <locale.h>

#include "gettextP.h"
#ifdef _LIBC
# include <libintl.h>
#else
# include "libgnuintl.h"
#endif

/* @@ end of prolog @@ */

/* Names for the libintl functions are a problem.  They must not clash
   with existing names and they should follow ANSI C.  But this source
   code is also used in GNU C Library where the names have a __
   prefix.  So we have to make a difference here.  */
#ifdef _LIBC
# define DGETTEXT __dgettext
# define DCGETTEXT INTUSE(__dcgettext)
#else
# define DGETTEXT dgettext__
# define DCGETTEXT dcgettext__
#endif

/* Look up MSGID in the DOMAINNAME message catalog of the current
   LC_MESSAGES locale.  */
char *DGETTEXT(domainname, msgid)
const char *domainname;
const char *msgid;
{
	return DCGETTEXT(domainname, msgid, LC_MESSAGES);
}

#ifdef _LIBC
/* Alias for function name in GNU C Library.  */
weak_alias(__dgettext, dgettext);
#endif
