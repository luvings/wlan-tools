/* Copyright (C) 1997, 1998, 1999, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Thorsten Kukuk <kukuk@vt.uni-paderborn.de>, 1997.

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
#include <unistd.h>
#include <string.h>
#include <rpc/rpc.h>

#include "nsswitch.h"

#define	OPSYS_LEN 4
#define	MAXIPRINT (11)		/* max length of printed integer */
static const char OPSYS[] = "unix";

int user2netname(char netname[MAXNETNAMELEN + 1], const uid_t uid,
                 const char *domain)
{
	char dfltdom[MAXNETNAMELEN + 1];
	size_t i;

	if (domain == NULL) {
		if (getdomainname(dfltdom, sizeof(dfltdom)) < 0)
			return 0;

	} else {
		strncpy(dfltdom, domain, MAXNETNAMELEN);
		dfltdom[MAXNETNAMELEN] = '\0';
	}

	if ((strlen(dfltdom) + OPSYS_LEN + 3 + MAXIPRINT) > (size_t) MAXNETNAMELEN)
		return 0;

	sprintf(netname, "%s.%d@%s", OPSYS, uid, dfltdom);
	i = strlen(netname);

	if (netname[i - 1] == '.')
		netname[i - 1] = '\0';

	return 1;
}

libc_hidden_def(user2netname)

int
host2netname(char netname[MAXNETNAMELEN + 1], const char *host,
             const char *domain)
{
	char *p;
	char hostname[MAXHOSTNAMELEN + 1];
	char domainname[MAXHOSTNAMELEN + 1];
	char *dot_in_host;
	size_t i;

	netname[0] = '\0';		/* make null first (no need for memset) */

	if (host == NULL)
		__gethostname(hostname, MAXHOSTNAMELEN);

	else {
		strncpy(hostname, host, MAXHOSTNAMELEN);
		hostname[MAXHOSTNAMELEN] = '\0';
	}

	dot_in_host = strchr(hostname, '.');

	if (domain == NULL) {
		p = dot_in_host;

		if (p) {
			++p;
			strncpy(domainname, p, MAXHOSTNAMELEN);
			domainname[MAXHOSTNAMELEN] = '\0';

		} else {
			domainname[0] = 0;
			getdomainname(domainname, MAXHOSTNAMELEN);
		}

	} else {
		strncpy(domainname, domain, MAXHOSTNAMELEN);
		domainname[MAXHOSTNAMELEN] = '\0';
	}

	i = strlen(domainname);

	if (i == 0)
		/* No domainname */
		return 0;

	if (domainname[i - 1] == '.')
		domainname[i - 1] = 0;

	if (dot_in_host)		/* strip off rest of name */
		*dot_in_host = '\0';

	if ((strlen(domainname) + strlen(hostname) + OPSYS_LEN + 3)
	                > MAXNETNAMELEN)
		return 0;

	sprintf(netname, "%s.%s@%s", OPSYS, hostname, domainname);
	return 1;
}

libc_hidden_def(host2netname)

int
getnetname(char name[MAXNETNAMELEN + 1])
{
	uid_t uid;
	int dummy;

	uid = __geteuid();

	if (uid == 0)
		dummy = host2netname(name, NULL, NULL);

	else
		dummy = user2netname(name, uid, NULL);

	return (dummy);
}

libc_hidden_def(getnetname)

/* Type of the lookup function for netname2user.  */
typedef int (*netname2user_function)(const char netname[MAXNETNAMELEN + 1],
                                     uid_t *, gid_t *, int *, gid_t *);
/* The lookup function for the first entry of this service.  */
extern int __nss_publickey_lookup(service_user ** nip, const char *name,
                                  void **fctp) internal_function;

int netname2user(const char netname[MAXNETNAMELEN + 1], uid_t * uidp, gid_t * gidp,
                 int *gidlenp, gid_t * gidlist)
{
	static service_user *startp;
	static netname2user_function start_fct;
	service_user *nip;
	netname2user_function fct;
	enum nss_status status = NSS_STATUS_UNAVAIL;
	int no_more;

	if (startp == NULL) {
		no_more = __nss_publickey_lookup(&nip, "netname2user", (void **) &fct);

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

	while (!no_more) {
		status = (*fct)(netname, uidp, gidp, gidlenp, gidlist);

		no_more = __nss_next(&nip, "netname2user", (void **) &fct, status, 0);
	}

	return status == NSS_STATUS_SUCCESS;
}

libc_hidden_def(netname2user)

int
netname2host(const char netname[MAXNETNAMELEN + 1], char *hostname,
             const int hostlen)
{
	char *p1, *p2;
	char buffer[MAXNETNAMELEN + 1];

	p1 = strchr(buffer, '.');

	if (p1 == NULL)
		return 0;

	p1++;

	p2 = strchr(p1, '@');

	if (p2 == NULL)
		return 0;

	*p2 = '\0';

	if (hostlen > MAXNETNAMELEN)
		return 0;

	strncpy(hostname, p1, hostlen);
	hostname[hostlen] = '\0';

	return 1;
}
