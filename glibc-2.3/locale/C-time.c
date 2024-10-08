/* Copyright (C) 1995-2000, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gnu.org>, 1995.

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

#include "localeinfo.h"

/* This table's entries are taken from POSIX.2 Table 2-11
   ``LC_TIME Category Definition in the POSIX Locale'',
   with additions from ISO 14652, section 4.6.  */

const struct locale_data _nl_C_LC_TIME attribute_hidden = {
	_nl_C_name,
	NULL, 0, 0,			/* no file mapped */
	{ NULL, },			/* no cached data */
	UNDELETABLE,
	0,
	111,
	{
		{ string: "Sun" },
		{ string: "Mon" },
		{ string: "Tue" },
		{ string: "Wed" },
		{ string: "Thu" },
		{ string: "Fri" },
		{ string: "Sat" },
		{ string: "Sunday" },
		{ string: "Monday" },
		{ string: "Tuesday" },
		{ string: "Wednesday" },
		{ string: "Thursday" },
		{ string: "Friday" },
		{ string: "Saturday" },
		{ string: "Jan" },
		{ string: "Feb" },
		{ string: "Mar" },
		{ string: "Apr" },
		{ string: "May" },
		{ string: "Jun" },
		{ string: "Jul" },
		{ string: "Aug" },
		{ string: "Sep" },
		{ string: "Oct" },
		{ string: "Nov" },
		{ string: "Dec" },
		{ string: "January" },
		{ string: "February" },
		{ string: "March" },
		{ string: "April" },
		{ string: "May" },
		{ string: "June" },
		{ string: "July" },
		{ string: "August" },
		{ string: "September" },
		{ string: "October" },
		{ string: "November" },
		{ string: "December" },
		{ string: "AM" },
		{ string: "PM" },
		{ string: "%a %b %e %H:%M:%S %Y" },
		{ string: "%m/%d/%y" },
		{ string: "%H:%M:%S" },
		{ string: "%I:%M:%S %p" },
		{ string: NULL },
		{ string: "" },
		{ string: "" },
		{ string: "" },
		{ string: "" },
		{ string: "" },
		{ word: 0 },
		{ string: "" },
		{ wstr: (const uint32_t *) L"Sun" },
		{ wstr: (const uint32_t *) L"Mon" },
		{ wstr: (const uint32_t *) L"Tue" },
		{ wstr: (const uint32_t *) L"Wed" },
		{ wstr: (const uint32_t *) L"Thu" },
		{ wstr: (const uint32_t *) L"Fri" },
		{ wstr: (const uint32_t *) L"Sat" },
		{ wstr: (const uint32_t *) L"Sunday" },
		{ wstr: (const uint32_t *) L"Monday" },
		{ wstr: (const uint32_t *) L"Tuesday" },
		{ wstr: (const uint32_t *) L"Wednesday" },
		{ wstr: (const uint32_t *) L"Thursday" },
		{ wstr: (const uint32_t *) L"Friday" },
		{ wstr: (const uint32_t *) L"Saturday" },
		{ wstr: (const uint32_t *) L"Jan" },
		{ wstr: (const uint32_t *) L"Feb" },
		{ wstr: (const uint32_t *) L"Mar" },
		{ wstr: (const uint32_t *) L"Apr" },
		{ wstr: (const uint32_t *) L"May" },
		{ wstr: (const uint32_t *) L"Jun" },
		{ wstr: (const uint32_t *) L"Jul" },
		{ wstr: (const uint32_t *) L"Aug" },
		{ wstr: (const uint32_t *) L"Sep" },
		{ wstr: (const uint32_t *) L"Oct" },
		{ wstr: (const uint32_t *) L"Nov" },
		{ wstr: (const uint32_t *) L"Dec" },
		{ wstr: (const uint32_t *) L"January" },
		{ wstr: (const uint32_t *) L"February" },
		{ wstr: (const uint32_t *) L"March" },
		{ wstr: (const uint32_t *) L"April" },
		{ wstr: (const uint32_t *) L"May" },
		{ wstr: (const uint32_t *) L"June" },
		{ wstr: (const uint32_t *) L"July" },
		{ wstr: (const uint32_t *) L"August" },
		{ wstr: (const uint32_t *) L"September" },
		{ wstr: (const uint32_t *) L"October" },
		{ wstr: (const uint32_t *) L"November" },
		{ wstr: (const uint32_t *) L"December" },
		{ wstr: (const uint32_t *) L"AM" },
		{ wstr: (const uint32_t *) L"PM" },
		{ wstr: (const uint32_t *) L"%a %b %e %H:%M:%S %Y" },
		{ wstr: (const uint32_t *) L"%m/%d/%y" },
		{ wstr: (const uint32_t *) L"%H:%M:%S" },
		{ wstr: (const uint32_t *) L"%I:%M:%S %p" },
		{ wstr: (const uint32_t *) L"" },
		{ wstr: (const uint32_t *) L"" },
		{ wstr: (const uint32_t *) L"" },
		{ wstr: (const uint32_t *) L"" },
		{ wstr: (const uint32_t *) L"" },
		{ string: "\7" },
		{ word: 19971130 },
		{ string: "\4" },
		{ string: "\7" },
		{ string: "\1" },
		{ string: "\1" },
		{ string: "" },
		{ string: "%a %b %e %H:%M:%S %Z %Y" },
		{ wstr: (const uint32_t *) L"%a %b %e %H:%M:%S %Z %Y" },
		{ string: _nl_C_codeset }
	}
};
