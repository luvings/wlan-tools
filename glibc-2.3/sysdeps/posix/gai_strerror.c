/* Copyright (C) 1997,2001,02 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Philip Blundell <pjb27@cam.ac.uk>, 1997.

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
#include <netdb.h>
#include <libintl.h>

static struct {
	int code;
	const char *msg;
}

values[] = {
	{ EAI_ADDRFAMILY, N_("Address family for hostname not supported") },
	{ EAI_AGAIN, N_("Temporary failure in name resolution") },
	{ EAI_BADFLAGS, N_("Bad value for ai_flags") },
	{ EAI_FAIL, N_("Non-recoverable failure in name resolution") },
	{ EAI_FAMILY, N_("ai_family not supported") },
	{ EAI_MEMORY, N_("Memory allocation failure") },
	{ EAI_NODATA, N_("No address associated with hostname") },
	{ EAI_NONAME, N_("Name or service not known") },
	{ EAI_SERVICE, N_("Servname not supported for ai_socktype") },
	{ EAI_SOCKTYPE, N_("ai_socktype not supported") },
	{ EAI_SYSTEM, N_("System error") },
	{ EAI_INPROGRESS, N_("Processing request in progress") },
	{ EAI_CANCELED, N_("Request canceled") },
	{ EAI_NOTCANCELED, N_("Request not canceled") },
	{ EAI_ALLDONE, N_("All requests done") },
	{ EAI_INTR, N_("Interrupted by a signal") }
};

const char *gai_strerror(int code)
{
	size_t i;

	for (i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
		if (values[i].code == code)
			return _(values[i].msg);

	return _("Unknown error");
}

libc_hidden_def(gai_strerror)
