/* Test listing of network interface addresses.
   Copyright (C) 2002 Free Software Foundation, Inc.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	int failures = 0;
	struct ifaddrs *ifaces, *ifa;

	if (getifaddrs(&ifaces) < 0) {
		if (errno != ENOSYS) {
			printf("Couldn't get any interfaces: %s.\n", strerror(errno));
			exit(1);
		}

		/* The function is simply not implemented.  */
		exit(0);
	}

	puts("\
Name           Flags   Address         Netmask         Broadcast/Destination");

	for (ifa = ifaces; ifa != NULL; ifa = ifa->ifa_next) {
		char abuf[64], mbuf[64], dbuf[64];
		inline const char *addr_string(struct sockaddr * sa, char *buf) {
			switch (sa->sa_family) {
			case AF_INET:
				return inet_ntop(AF_INET,
				                 &((struct sockaddr_in *) sa)->sin_addr,
				                 buf, sizeof abuf);

			case AF_INET6:
				return inet_ntop(AF_INET6,
				                 &((struct sockaddr_in6 *) sa)->sin6_addr,
				                 buf, sizeof abuf);

			case AF_UNSPEC:
				return "---";

			default:
				++failures;
				printf("sa_family=%d %08x\n", sa->sa_family,
				       *(int*) & ((struct sockaddr_in *) sa)->sin_addr.s_addr);
				return "<unexpected sockaddr family>";
			}
		}

		printf("%-15s%#.4x  %-15s %-15s %-15s\n",
		       ifa->ifa_name, ifa->ifa_flags,
		       addr_string(ifa->ifa_addr, abuf),
		       addr_string(ifa->ifa_netmask, mbuf),
		       addr_string(ifa->ifa_broadaddr, dbuf));
	}

	freeifaddrs(ifaces);

	return failures ? 1 : 0;
}
