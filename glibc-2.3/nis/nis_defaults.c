/* Copyright (c) 1997, 1998 Free Software Foundation, Inc.
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <rpc/rpc.h>
#include <rpcsvc/nis.h>

#define DEFAULT_TTL 43200

/*
** Some functions for parsing the -D param and NIS_DEFAULTS Environ
*/
static nis_name searchgroup(char *str)
{
	char *cptr;
	int i;

	cptr = strstr(str, "group=");

	if (cptr == NULL)
		return NULL;

	cptr += 6;			/* points to the begin of the group string */
	i = 0;

	while (cptr[i] != '\0' && cptr[i] != ':')
		i++;

	if (i == 0)			/* only "group=" ? */
		return (nis_name) "";

	return strndup(cptr, i);
}

static nis_name searchowner(char *str)
{
	char *cptr;
	int i;

	cptr = strstr(str, "owner=");

	if (cptr == NULL)
		return NULL;

	cptr += 6;			/* points to the begin of the owner string */
	i = 0;

	while (cptr[i] != '\0' && cptr[i] != ':')
		i++;

	if (i == 0)			/* only "owner=" ? */
		return strdup("");

	return strndup(cptr, i);
}

static uint32_t searchttl(char *str)
{
	char buf[strlen(str) + 1];
	char *cptr, *dptr;
	uint32_t time;
	int i;

	dptr = strstr(str, "ttl=");

	if (dptr == NULL)		/* should (could) not happen */
		return DEFAULT_TTL;;

	dptr += 4;			/* points to the begin of the new ttl */
	i = 0;

	while (dptr[i] != '\0' && dptr[i] != ':')
		i++;

	if (i == 0)			/* only "ttl=" ? */
		return DEFAULT_TTL;

	strncpy(buf, dptr, i);
	buf[i] = '\0';
	time = 0;

	dptr = buf;
	cptr = strchr(dptr, 'd');

	if (cptr != NULL) {
		*cptr = '\0';
		cptr++;
		time += atoi(dptr) * 60 * 60 * 24;
		dptr = cptr;
	}

	cptr = strchr(dptr, 'h');

	if (cptr != NULL) {
		*cptr = '\0';
		cptr++;
		time += atoi(dptr) * 60 * 60;
		dptr = cptr;
	}

	cptr = strchr(dptr, 'm');

	if (cptr != NULL) {
		*cptr = '\0';
		cptr++;
		time += atoi(dptr) * 60;
		dptr = cptr;
	}

	cptr = strchr(dptr, 's');

	if (cptr != NULL)
		*cptr = '\0';

	time += atoi(dptr);

	return time;
}

static unsigned int searchaccess(char *str, unsigned int access)
{
	char buf[strlen(str) + 1];
	char *cptr;
	unsigned int result = access;
	int i;
	int n, o, g, w;

	cptr = strstr(str, "access=");

	if (cptr == NULL)
		return 0;

	cptr += 7;			/* points to the begin of the access string */
	i = 0;

	while (cptr[i] != '\0' && cptr[i] != ':')
		i++;

	if (i == 0)			/* only "access=" ? */
		return 0;

	strncpy(buf, cptr, i);
	buf[i] = '\0';

	n = o = g = w = 0;
	cptr = buf;

	if (*cptr == ',') /* Fix for stupid Solaris scripts */
		++cptr;

	while (*cptr != '\0') {
		switch (*cptr) {
		case 'n':
			n = 1;
			break;

		case 'o':
			o = 1;
			break;

		case 'g':
			g = 1;
			break;

		case 'w':
			w = 1;
			break;

		case 'a':
			o = g = w = 1;
			break;

		case '-':
			cptr++;		/* Remove "-" from beginning */

			while (*cptr != '\0' && *cptr != ',') {
				switch (*cptr) {
				case 'r':
					if (n)
						result = result & ~(NIS_READ_ACC << 24);

					if (o)
						result = result & ~(NIS_READ_ACC << 16);

					if (g)
						result = result & ~(NIS_READ_ACC << 8);

					if (w)
						result = result & ~(NIS_READ_ACC);

					break;

				case 'm':
					if (n)
						result = result & ~(NIS_MODIFY_ACC << 24);

					if (o)
						result = result & ~(NIS_MODIFY_ACC << 16);

					if (g)
						result = result & ~(NIS_MODIFY_ACC << 8);

					if (w)
						result = result & ~(NIS_MODIFY_ACC);

					break;

				case 'c':
					if (n)
						result = result & ~(NIS_CREATE_ACC << 24);

					if (o)
						result = result & ~(NIS_CREATE_ACC << 16);

					if (g)
						result = result & ~(NIS_CREATE_ACC << 8);

					if (w)
						result = result & ~(NIS_CREATE_ACC);

					break;

				case 'd':
					if (n)
						result = result & ~(NIS_DESTROY_ACC << 24);

					if (o)
						result = result & ~(NIS_DESTROY_ACC << 16);

					if (g)
						result = result & ~(NIS_DESTROY_ACC << 8);

					if (w)
						result = result & ~(NIS_DESTROY_ACC);

					break;

				default:
					return (~0U);
				}

				cptr++;
			}

			n = o = g = w = 0;
			break;

		case '+':
			cptr++;		/* Remove "+" from beginning */

			while (*cptr != '\0' && *cptr != ',') {
				switch (*cptr) {
				case 'r':
					if (n)
						result = result | (NIS_READ_ACC << 24);

					if (o)
						result = result | (NIS_READ_ACC << 16);

					if (g)
						result = result | (NIS_READ_ACC << 8);

					if (w)
						result = result | (NIS_READ_ACC);

					break;

				case 'm':
					if (n)
						result = result | (NIS_MODIFY_ACC << 24);

					if (o)
						result = result | (NIS_MODIFY_ACC << 16);

					if (g)
						result = result | (NIS_MODIFY_ACC << 8);

					if (w)
						result = result | (NIS_MODIFY_ACC);

					break;

				case 'c':
					if (n)
						result = result | (NIS_CREATE_ACC << 24);

					if (o)
						result = result | (NIS_CREATE_ACC << 16);

					if (g)
						result = result | (NIS_CREATE_ACC << 8);

					if (w)
						result = result | (NIS_CREATE_ACC);

					break;

				case 'd':
					if (n)
						result = result | (NIS_DESTROY_ACC << 24);

					if (o)
						result = result | (NIS_DESTROY_ACC << 16);

					if (g)
						result = result | (NIS_DESTROY_ACC << 8);

					if (w)
						result = result | (NIS_DESTROY_ACC);

					break;

				default:
					return (~0U);
				}

				cptr++;
			}

			n = o = g = w = 0;
			break;

		case '=':
			cptr++;		/* Remove "=" from beginning */

			/* Clear */
			if (n)
				result = result & ~((NIS_READ_ACC + NIS_MODIFY_ACC +
				                     NIS_CREATE_ACC + NIS_DESTROY_ACC) << 24);

			if (o)
				result = result & ~((NIS_READ_ACC + NIS_MODIFY_ACC +
				                     NIS_CREATE_ACC + NIS_DESTROY_ACC) << 16);

			if (g)
				result = result & ~((NIS_READ_ACC + NIS_MODIFY_ACC +
				                     NIS_CREATE_ACC + NIS_DESTROY_ACC) << 8);

			if (w)
				result = result & ~(NIS_READ_ACC + NIS_MODIFY_ACC +
				                    NIS_CREATE_ACC + NIS_DESTROY_ACC);

			while (*cptr != '\0' && *cptr != ',') {
				switch (*cptr) {
				case 'r':
					if (n)
						result = result | (NIS_READ_ACC << 24);

					if (o)
						result = result | (NIS_READ_ACC << 16);

					if (g)
						result = result | (NIS_READ_ACC << 8);

					if (w)
						result = result | (NIS_READ_ACC);

					break;

				case 'm':
					if (n)
						result = result | (NIS_MODIFY_ACC << 24);

					if (o)
						result = result | (NIS_MODIFY_ACC << 16);

					if (g)
						result = result | (NIS_MODIFY_ACC << 8);

					if (w)
						result = result | (NIS_MODIFY_ACC);

					break;

				case 'c':
					if (n)
						result = result | (NIS_CREATE_ACC << 24);

					if (o)
						result = result | (NIS_CREATE_ACC << 16);

					if (g)
						result = result | (NIS_CREATE_ACC << 8);

					if (w)
						result = result | (NIS_CREATE_ACC);

					break;

				case 'd':
					if (n)
						result = result | (NIS_DESTROY_ACC << 24);

					if (o)
						result = result | (NIS_DESTROY_ACC << 16);

					if (g)
						result = result | (NIS_DESTROY_ACC << 8);

					if (w)
						result = result | (NIS_DESTROY_ACC);

					break;

				default:
					return result = (~0U);
				}

				cptr++;
			}

			n = o = g = w = 0;
			break;

		default:
			return result = (~0U);
		}

		if (*cptr != '\0')
			cptr++;
	}

	return result;
}

nis_name __nis_default_owner(char *defaults)
{
	char default_owner[NIS_MAXNAMELEN + 1];
	char *cptr, *dptr;

	strcpy(default_owner, nis_local_principal());

	if (defaults != NULL) {
		dptr = strstr(defaults, "owner=");

		if (dptr != NULL) {
			char *p = searchowner(defaults);

			if (strlen(p) <= NIS_MAXNAMELEN)
				strcpy(default_owner, p);

			free(p);
		}

	} else {
		cptr = getenv("NIS_DEFAULTS");

		if (cptr != NULL) {
			dptr = strstr(cptr, "owner=");

			if (dptr != NULL) {
				char *p = searchowner(cptr);

				if (strlen(p) <= NIS_MAXNAMELEN)
					strcpy(default_owner, p);

				free(p);
			}
		}
	}

	return strdup(default_owner);
}

nis_name __nis_default_group(char *defaults)
{
	char default_group[NIS_MAXNAMELEN + 1];
	char *cptr, *dptr;

	strcpy(default_group, nis_local_group());

	if (defaults != NULL) {
		dptr = strstr(defaults, "group=");

		if (dptr != NULL) {
			char *p = searchgroup(defaults);

			if (strlen(p) <= NIS_MAXNAMELEN)
				strcpy(default_group, p);

			free(p);
		}

	} else {
		cptr = getenv("NIS_DEFAULTS");

		if (cptr != NULL) {
			dptr = strstr(cptr, "group=");

			if (dptr != NULL) {
				char *p = searchgroup(cptr);

				if (strlen(p) <= NIS_MAXNAMELEN)
					strcpy(default_group, p);

				free(p);
			}
		}
	}

	return strdup(default_group);
}

uint32_t __nis_default_ttl(char *defaults)
{
	char *cptr, *dptr;

	if (defaults != NULL) {
		dptr = strstr(defaults, "ttl=");

		if (dptr != NULL)
			return searchttl(defaults);
	}

	cptr = getenv("NIS_DEFAULTS");

	if (cptr == NULL)
		return DEFAULT_TTL;

	dptr = strstr(cptr, "ttl=");

	if (dptr == NULL)
		return DEFAULT_TTL;

	return searchttl(cptr);
}

/* Default access rights are ----rmcdr---r---, but we could change
   this with the NIS_DEFAULTS variable. */
unsigned int __nis_default_access(char *param, unsigned int defaults)
{
	unsigned int result;
	char *cptr;

	if (defaults == 0)
		result = 0 | OWNER_DEFAULT | GROUP_DEFAULT | WORLD_DEFAULT;

	else
		result = defaults;

	if (param != NULL && strstr(param, "access=") != NULL)
		result = searchaccess(param, result);

	else {
		cptr = getenv("NIS_DEFAULTS");

		if (cptr != NULL && strstr(cptr, "access=") != NULL)
			result = searchaccess(getenv("NIS_DEFAULTS"), result);
	}

	return result;
}
