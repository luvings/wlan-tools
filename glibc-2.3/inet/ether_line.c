/* Copyright (C) 1996, 1999, 2002 Free Software Foundation, Inc.
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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>


int ether_line(const char *line, struct ether_addr *addr, char *hostname)
{
	size_t cnt;
	char *cp;

	for (cnt = 0; cnt < 6; ++cnt) {
		unsigned int number;
		char ch;

		ch = _tolower(*line++);

		if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
			return -1;

		number = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

		ch = _tolower(*line);

		if ((cnt < 5 && ch != ':') || (cnt == 5 && ch != '\0' && !isspace(ch))) {
			++line;

			if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
				return -1;

			number <<= 4;
			number += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

			ch = *line;

			if (cnt < 5 && ch != ':')
				return -1;
		}

		/* Store result.  */
		addr->ether_addr_octet[cnt] = (unsigned char) number;

		/* Skip ':'.  */
		if (ch != '\0')
			++line;
	}

	/* Remove trailing white space.  */
	cp = __strchrnul(line, '#');

	while (cp > line && isspace(cp[-1]))
		--cp;

	if (cp == line)
		/* No hostname.  */
		return -1;

	/* XXX This can cause trouble because the hostname might be too long
	   but we have no possibility to check it here.  */
	memcpy(hostname, line, cp - line);
	hostname [cp - line] = '\0';

	return 0;
}
