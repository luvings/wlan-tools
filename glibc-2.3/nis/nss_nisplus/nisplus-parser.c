/* Copyright (C) 1997, 1999 Free Software Foundation, Inc.
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

#include <pwd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <rpcsvc/nis.h>

#include "nisplus-parser.h"

#define NISENTRYVAL(idx,col,res) \
        ((res)->objects.objects_val[(idx)].EN_data.en_cols.en_cols_val[(col)].ec_value.ec_value_val)

#define NISENTRYLEN(idx,col,res) \
        ((res)->objects.objects_val[(idx)].EN_data.en_cols.en_cols_val[(col)].ec_value.ec_value_len)


int _nss_nisplus_parse_pwent(nis_result *result, struct passwd *pw,
                             char *buffer, size_t buflen, int *errnop)
{
	char *first_unused = buffer;
	size_t room_left = buflen;
	size_t len;

	if (result == NULL)
		return 0;

	if ((result->status != NIS_SUCCESS && result->status != NIS_S_SUCCESS)
	                || result->objects.objects_len != 1
	                || __type_of(result->objects.objects_val) != NIS_ENTRY_OBJ
	                || strcmp(result->objects.objects_val->EN_data.en_type,
	                          "passwd_tbl") != 0
	                || result->objects.objects_val->EN_data.en_cols.en_cols_len < 7)
		return 0;

	if (NISENTRYLEN(0, 0, result) >= room_left) {
		/* The line is too long for our buffer.  */
no_more_room:
		*errnop = ERANGE;
		return -1;
	}

	strncpy(first_unused, NISENTRYVAL(0, 0, result),
	        NISENTRYLEN(0, 0, result));
	first_unused[NISENTRYLEN(0, 0, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0) /* No name ? Should never happen, database is corrupt */
		return 0;

	pw->pw_name = first_unused;
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 1, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 1, result),
	        NISENTRYLEN(0, 1, result));
	first_unused[NISENTRYLEN(0, 1, result)] = '\0';
	pw->pw_passwd = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 2, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 2, result),
	        NISENTRYLEN(0, 2, result));
	first_unused[NISENTRYLEN(0, 2, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0) /* If we don't have a uid, it's an invalid shadow entry */
		return 0;

	pw->pw_uid = strtoul(first_unused, NULL, 10);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 3, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 3, result),
	        NISENTRYLEN(0, 3, result));
	first_unused[NISENTRYLEN(0, 3, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0) /* If we don't have a gid, it's an invalid shadow entry */
		return 0;

	pw->pw_gid = strtoul(first_unused, NULL, 10);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 4, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 4, result),
	        NISENTRYLEN(0, 4, result));
	first_unused[NISENTRYLEN(0, 4, result)] = '\0';
	pw->pw_gecos = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 5, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 5, result),
	        NISENTRYLEN(0, 5, result));
	first_unused[NISENTRYLEN(0, 5, result)] = '\0';
	pw->pw_dir = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 6, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 6, result),
	        NISENTRYLEN(0, 6, result));
	first_unused[NISENTRYLEN(0, 6, result)] = '\0';
	pw->pw_shell = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	return 1;
}

int _nss_nisplus_parse_grent(nis_result *result, u_long entry, struct group *gr,
                             char *buffer, size_t buflen, int *errnop)
{
	char *first_unused = buffer;
	size_t room_left = buflen;
	char *line;
	int count;
	size_t len;

	if (result == NULL)
		return 0;

	if ((result->status != NIS_SUCCESS && result->status != NIS_S_SUCCESS)
	                || __type_of(result->objects.objects_val) != NIS_ENTRY_OBJ
	                || strcmp(result->objects.objects_val[entry].EN_data.en_type,
	                          "group_tbl") != 0
	                || result->objects.objects_val[entry].EN_data.en_cols.en_cols_len < 4)
		return 0;

	if (NISENTRYLEN(entry, 0, result) >= room_left) {
		/* The line is too long for our buffer.  */
no_more_room:
		*errnop = ERANGE;
		return -1;
	}

	strncpy(first_unused, NISENTRYVAL(entry, 0, result),
	        NISENTRYLEN(entry, 0, result));
	first_unused[NISENTRYLEN(entry, 0, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0) /* group table is corrupt */
		return 0;

	gr->gr_name = first_unused;
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(entry, 1, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(entry, 1, result),
	        NISENTRYLEN(entry, 1, result));
	first_unused[NISENTRYLEN(entry, 1, result)] = '\0';
	gr->gr_passwd = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(entry, 2, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(entry, 2, result),
	        NISENTRYLEN(entry, 2, result));
	first_unused[NISENTRYLEN(entry, 2, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0) /* We should always have an gid */
		return 0;

	gr->gr_gid = strtoul(first_unused, NULL, 10);
	room_left -= (strlen(first_unused) + 1);
	first_unused += strlen(first_unused) + 1;

	if (NISENTRYLEN(entry, 3, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(entry, 3, result),
	        NISENTRYLEN(entry, 3, result));
	first_unused[NISENTRYLEN(entry, 3, result)] = '\0';
	line = first_unused;
	len = strlen(line);
	room_left -= (len + 1);
	first_unused += (len + 1);
	/* Adjust the pointer so it is aligned for
	   storing pointers.  */
	first_unused += __alignof__(char *) - 1;
	first_unused -= ((first_unused - (char *) 0) % __alignof__(char *));
	gr->gr_mem = (char **) first_unused;

	count = 0;

	while (*line != '\0') {
		/* Skip leading blanks.  */
		while (isspace(*line))
			++line;

		if (*line == '\0')
			break;

		if (room_left < sizeof(char *))
			goto no_more_room;

		room_left -= sizeof(char *);
		gr->gr_mem[count++] = line;

		while (*line != '\0' && *line != ',' && !isspace(*line))
			++line;

		if (*line == ',' || isspace(*line)) {
			int is = isspace(*line);

			*line = '\0';

			if (is)
				while (*line != '\0' && (*line == ',' || isspace(*line)))
					++line;

			else
				++line;
		}
	}

	if (room_left < sizeof(char *))
		goto no_more_room;

	room_left -= sizeof(char *);
	gr->gr_mem[count] = NULL;

	return 1;
}

int _nss_nisplus_parse_spent(nis_result *result, struct spwd *sp,
                             char *buffer, size_t buflen, int *errnop)
{
	char *first_unused = buffer;
	size_t room_left = buflen;
	size_t len;

	if (result == NULL)
		return 0;

	if ((result->status != NIS_SUCCESS && result->status != NIS_S_SUCCESS)
	                || result->objects.objects_len != 1
	                || __type_of(result->objects.objects_val) != NIS_ENTRY_OBJ
	                || strcmp(result->objects.objects_val->EN_data.en_type,
	                          "passwd_tbl") != 0
	                || result->objects.objects_val[0].EN_data.en_cols.en_cols_len < 8)
		return 0;

	if (NISENTRYLEN(0, 0, result) >= room_left) {
		/* The line is too long for our buffer.  */
no_more_room:
		*errnop = ERANGE;
		return -1;
	}

	strncpy(first_unused, NISENTRYVAL(0, 0, result),
	        NISENTRYLEN(0, 0, result));
	first_unused[NISENTRYLEN(0, 0, result)] = '\0';
	len = strlen(first_unused);

	if (len == 0)
		return 0;

	sp->sp_namp = first_unused;
	room_left -= (len + 1);
	first_unused += (len + 1);

	if (NISENTRYLEN(0, 1, result) >= room_left)
		goto no_more_room;

	strncpy(first_unused, NISENTRYVAL(0, 1, result),
	        NISENTRYLEN(0, 1, result));
	first_unused[NISENTRYLEN(0, 1, result)] = '\0';
	sp->sp_pwdp = first_unused;
	len = strlen(first_unused);
	room_left -= (len + 1);
	first_unused += (len + 1);

	sp->sp_lstchg = sp->sp_min = sp->sp_max = sp->sp_warn = sp->sp_inact =
	                                     sp->sp_expire = -1;
	sp->sp_flag = ~0ul;

	if (NISENTRYLEN(0, 7, result) > 0) {
		char *line, *cp;

		line = NISENTRYVAL(0, 7, result);
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_lstchg = atol(line);

		line = cp;
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_min = atol(line);

		line = cp;
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_max = atol(line);

		line = cp;
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_warn = atol(line);

		line = cp;
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_inact = atol(line);

		line = cp;
		cp = strchr(line, ':');

		if (cp == NULL)
			return 1;

		*cp++ = '\0';

		if (*line)
			sp->sp_expire = atol(line);

		line = cp;

		if (line == NULL)
			return 1;

		if (*line)
			sp->sp_flag = atol(line);
	}

	return 1;
}
