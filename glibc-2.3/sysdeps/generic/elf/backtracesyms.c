/* Return list with names for address in backtrace.
   Copyright (C) 1998, 1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

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
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ldsodefs.h>

#if __ELF_NATIVE_CLASS == 32
# define WORD_WIDTH 8
#else
/* We assyme 64bits.  */
# define WORD_WIDTH 16
#endif


char **__backtrace_symbols(array, size)
void *const *array;
int size;
{
	Dl_info info[size];
	int status[size];
	int cnt;
	size_t total = 0;
	char **result;

	/* Fill in the information we can get from `dladdr'.  */
	for (cnt = 0; cnt < size; ++cnt) {
		status[cnt] = _dl_addr(array[cnt], &info[cnt]);

		if (status[cnt] && info[cnt].dli_fname && info[cnt].dli_fname[0] != '\0')
			/* We have some info, compute the length of the string which will be
			   "<file-name>(<sym-name>) [+offset].  */
			total += (strlen(info[cnt].dli_fname ? : "")
			          + (info[cnt].dli_sname
			             ? strlen(info[cnt].dli_sname) + 3 + WORD_WIDTH + 3
			             : 1)
			          + WORD_WIDTH + 5);

		else
			total += 5 + WORD_WIDTH;
	}

	/* Allocate memory for the result.  */
	result = (char **) malloc(size * sizeof(char *) + total);

	if (result != NULL) {
		char *last = (char *)(result + size);

		for (cnt = 0; cnt < size; ++cnt) {
			result[cnt] = last;

			if (status[cnt] && info[cnt].dli_fname
			                && info[cnt].dli_fname[0] != '\0') {
				char buf[20];

				if (array[cnt] >= (void *) info[cnt].dli_saddr)
					sprintf(buf, "+%#lx",
					        (unsigned long)(array[cnt] - info[cnt].dli_saddr));

				else
					sprintf(buf, "-%#lx",
					        (unsigned long)(info[cnt].dli_saddr - array[cnt]));

				last += 1 + sprintf(last, "%s%s%s%s%s[%p]",
				                    info[cnt].dli_fname ? : "",
				                    info[cnt].dli_sname ? "(" : "",
				                    info[cnt].dli_sname ? : "",
				                    info[cnt].dli_sname ? buf : "",
				                    info[cnt].dli_sname ? ") " : " ",
				                    array[cnt]);

			} else
				last += 1 + sprintf(last, "[%p]", array[cnt]);
		}

		assert(last <= (char *) result + size * sizeof(char *) + total);
	}

	return result;
}

weak_alias(__backtrace_symbols, backtrace_symbols)
