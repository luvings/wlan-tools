/* Copyright (C) 1997, 1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by H.J. Lu <hjl@gnu.ai.mit.edu>, 1997.

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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wctype.h>
#include <resolv.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "nsswitch.h"

#ifdef USE_NSCD
# define inet_aton __inet_aton
# include <nscd/nscd_proto.h>
#endif

int __nss_hostname_digits_dots(const char *name, struct hostent *resbuf,
                               char **buffer, size_t *buffer_size,
                               size_t buflen, struct hostent **result,
                               enum nss_status *status, int *typep,
                               int flags, int *afp, int *h_errnop)
{
	int save;

	/* We have to test for the use of IPv6 which can only be done by
	   examining `_res'.  */
	if ((_res.options & RES_INIT) == 0 && __res_ninit(&_res) == -1) {
		if (h_errnop)
			*h_errnop = NETDB_INTERNAL;

		*result = NULL;
		return -1;
	}

	/*
	 * disallow names consisting only of digits/dots, unless
	 * they end in a dot.
	 */
	if (isdigit(name[0]) || isxdigit(name[0]) || name[0] == ':') {
		const char *cp;
		char *hostname;
		typedef unsigned char host_addr_t[16];
		host_addr_t *host_addr;
		typedef char *host_addr_list_t[2];
		host_addr_list_t *h_addr_ptrs;
		char **h_alias_ptr;
		size_t size_needed;
		int addr_size;
		int af;

		if (typep != NULL)
			af = *typep;

		else if (afp != NULL)
			af = *afp;

		else
			af = -1;

		switch (af) {
		case AF_INET:
			addr_size = INADDRSZ;
			break;

		case AF_INET6:
			addr_size = IN6ADDRSZ;
			break;

		default:
			if (typep != NULL) {
				/* This must not happen.  */
				if (h_errnop != NULL)
					*h_errnop = HOST_NOT_FOUND;

				goto done;

			} else {
				af = (_res.options & RES_USE_INET6) ? AF_INET6 : AF_INET;
				addr_size = af == AF_INET6 ? IN6ADDRSZ : INADDRSZ;
			}

			break;
		}

		size_needed = (sizeof(*host_addr)
		               + sizeof(*h_addr_ptrs) + strlen(name) + 1);

		if (buffer_size == NULL) {
			if (buflen < size_needed) {
				if (h_errnop != NULL)
					*h_errnop = TRY_AGAIN;

				__set_errno(ERANGE);
				goto done;
			}

		} else if (buffer_size != NULL && *buffer_size < size_needed) {
			char *new_buf;
			*buffer_size = size_needed;
			new_buf = (char *) realloc(*buffer, *buffer_size);

			if (new_buf == NULL) {
				save = errno;
				free(*buffer);
				*buffer = NULL;
				*buffer_size = 0;
				__set_errno(save);

				if (h_errnop != NULL)
					*h_errnop = TRY_AGAIN;

				*result = NULL;
				goto done;
			}

			*buffer = new_buf;
		}

		memset(*buffer, '\0', size_needed);

		host_addr = (host_addr_t *) *buffer;
		h_addr_ptrs = (host_addr_list_t *)
		              ((char *) host_addr + sizeof(*host_addr));
		h_alias_ptr = (char **)((char *) h_addr_ptrs + sizeof(*h_addr_ptrs));
		hostname = (char *) h_alias_ptr + sizeof(*h_alias_ptr);

		if (isdigit(name[0])) {
			for (cp = name;; ++cp) {
				if (*cp == '\0') {
					int ok;

					if (*--cp == '.')
						break;

					/* All-numeric, no dot at the end. Fake up a hostent as if
					   we'd actually done a lookup.  What if someone types
					   255.255.255.255?  The test below will succeed
					   spuriously... ???  */
					if (af == AF_INET)
						ok = __inet_aton(name, (struct in_addr *) host_addr);

					else {
						assert(af == AF_INET6);
						ok = inet_pton(af, name, host_addr) > 0;
					}

					if (! ok) {
						*h_errnop = HOST_NOT_FOUND;

						if (buffer_size)
							*result = NULL;

						goto done;
					}

					resbuf->h_name = strcpy(hostname, name);
					h_alias_ptr[0] = NULL;
					resbuf->h_aliases = h_alias_ptr;
					(*h_addr_ptrs)[0] = (char *) host_addr;
					(*h_addr_ptrs)[1] = NULL;
					resbuf->h_addr_list = *h_addr_ptrs;

					if ((typep != NULL && *typep == AF_INET6)
					                || (af == AF_INET
					                    && (_res.options & RES_USE_INET6))) {
						if (typep != NULL && (flags & AI_V4MAPPED) == 0) {
							/* That's bad.  The user hasn't specified that she
							   allows IPv4 numeric addresses.  */
							*result = NULL;
							*h_errnop = HOST_NOT_FOUND;
							goto done;

						} else {
							/* We need to change the IP v4 address into the
							   IP v6 address.  */
							char tmp[INADDRSZ];
							char *p = (char *) host_addr;
							int i;

							/* Save a copy of the IP v4 address. */
							memcpy(tmp, host_addr, INADDRSZ);

							/* Mark this ipv6 addr as a mapped ipv4. */
							for (i = 0; i < 10; i++)
								*p++ = 0x00;

							*p++ = 0xff;
							*p++ = 0xff;
							/* Copy the IP v4 address. */
							memcpy(p, tmp, INADDRSZ);
							resbuf->h_addrtype = AF_INET6;
							resbuf->h_length = IN6ADDRSZ;
						}

					} else {
						resbuf->h_addrtype = af;
						resbuf->h_length = addr_size;
					}

					if (h_errnop != NULL)
						*h_errnop = NETDB_SUCCESS;

					if (buffer_size == NULL)
						*status = NSS_STATUS_SUCCESS;

					else
						*result = resbuf;

					goto done;
				}

				if (!isdigit(*cp) && *cp != '.')
					break;
			}
		}

		if ((isxdigit(name[0]) && strchr(name, ':') != NULL) || name[0] == ':') {
			const char *cp;
			char *hostname;
			typedef unsigned char host_addr_t[16];
			host_addr_t *host_addr;
			typedef char *host_addr_list_t[2];
			host_addr_list_t *h_addr_ptrs;
			size_t size_needed;
			int addr_size;
			int af;

			if (typep != NULL)
				af = *typep;

			else if (afp != NULL)
				af = *afp;

			else
				af = -1;

			switch (af) {
			default:
				af = (_res.options & RES_USE_INET6) ? AF_INET6 : AF_INET;

				if (af == AF_INET6) {
					addr_size = IN6ADDRSZ;
					break;
				}

			/* FALLTHROUGH */

			case AF_INET:
				/* This is not possible.  We cannot represent an IPv6 address
				 in an `struct in_addr' variable.  */
				*h_errnop = HOST_NOT_FOUND;
				*result = NULL;
				goto done;

			case AF_INET6:
				addr_size = IN6ADDRSZ;
				break;
			}

			size_needed = (sizeof(*host_addr)
			               + sizeof(*h_addr_ptrs) + strlen(name) + 1);

			if (buffer_size == NULL && buflen < size_needed) {
				if (h_errnop != NULL)
					*h_errnop = TRY_AGAIN;

				__set_errno(ERANGE);
				goto done;

			} else if (buffer_size != NULL && *buffer_size < size_needed) {
				char *new_buf;
				*buffer_size = size_needed;
				new_buf = realloc(*buffer, *buffer_size);

				if (new_buf == NULL) {
					save = errno;
					free(*buffer);
					__set_errno(save);
					*buffer = NULL;
					*buffer_size = 0;
					*result = NULL;
					goto done;
				}

				*buffer = new_buf;
			}

			memset(*buffer, '\0', size_needed);

			host_addr = (host_addr_t *) *buffer;
			h_addr_ptrs = (host_addr_list_t *)
			              ((char *) host_addr + sizeof(*host_addr));
			hostname = (char *) h_addr_ptrs + sizeof(*h_addr_ptrs);

			for (cp = name;; ++cp) {
				if (!*cp) {
					if (*--cp == '.')
						break;

					/* All-IPv6-legal, no dot at the end. Fake up a
					   hostent as if we'd actually done a lookup.  */
					if (inet_pton(AF_INET6, name, host_addr) <= 0) {
						*h_errnop = HOST_NOT_FOUND;

						if (buffer_size)
							*result = NULL;

						goto done;
					}

					resbuf->h_name = strcpy(hostname, name);
					h_alias_ptr[0] = NULL;
					resbuf->h_aliases = h_alias_ptr;
					(*h_addr_ptrs)[0] = (char *) host_addr;
					(*h_addr_ptrs)[1] = (char *) 0;
					resbuf->h_addr_list = *h_addr_ptrs;
					resbuf->h_addrtype = AF_INET6;
					resbuf->h_length = addr_size;
					*h_errnop = NETDB_SUCCESS;

					if (buffer_size == NULL)
						*status = NSS_STATUS_SUCCESS;

					else
						*result = resbuf;

					goto done;
				}

				if (!isxdigit(*cp) && *cp != ':' && *cp != '.')
					break;
			}
		}
	}

	return 0;

done:
	return 1;
}

libc_hidden_def(__nss_hostname_digits_dots)
