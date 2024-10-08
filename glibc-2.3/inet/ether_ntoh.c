/* Copyright (C) 1996, 1997, 1999, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

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
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <string.h>

#include <nss/nsswitch.h>


/* Type of the lookup function we need here.  */
typedef int (*lookup_function)(const struct ether_addr *, struct etherent *,
                               char *, size_t, int *);

/* The lookup function for the first entry of this service.  */
extern int __nss_ethers_lookup(service_user **nip, const char *name,
                               void **fctp) internal_function;


int ether_ntohost(char *hostname, const struct ether_addr *addr)
{
	static service_user *startp;
	static lookup_function start_fct;
	service_user *nip;
	lookup_function fct;
	int no_more;
	enum nss_status status = NSS_STATUS_UNAVAIL;
	struct etherent etherent;

	if (startp == NULL) {
		no_more = __nss_ethers_lookup(&nip, "getntohost_r", (void **) &fct);

		if (no_more)
			startp = (service_user *) - 1;

		else {
			startp = nip;
			start_fct = fct;
		}

	} else {
		fct = start_fct;
		no_more = (nip = startp) == (service_user *) - 1;
	}

	while (no_more == 0) {
		char buffer[1024];

		status = (*fct)(addr, &etherent, buffer, sizeof buffer, &errno);

		no_more = __nss_next(&nip, "getntohost_r", (void **) &fct, status, 0);
	}

	if (status == NSS_STATUS_SUCCESS)
		/* XXX This is a potential cause of trouble because the size of
		   the HOSTNAME buffer is not known but the interface does not
		   provide this information.  */
		strcpy(hostname, etherent.e_name);

	return status == NSS_STATUS_SUCCESS ? 0 : -1;
}
