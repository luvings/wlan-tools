/* Copyright (C) 1991, 92, 96, 97, 98, 99, 2001 Free Software Foundation, Inc.
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

#ifndef	_WORDEXP_H
#define	_WORDEXP_H	1

#include <features.h>
#define __need_size_t
#include <stddef.h>

__BEGIN_DECLS

/* Bits set in the FLAGS argument to `wordexp'.  */
enum {
	WRDE_DOOFFS = (1 << 0),	/* Insert PWORDEXP->we_offs NULLs.  */
	WRDE_APPEND = (1 << 1),	/* Append to results of a previous call.  */
	WRDE_NOCMD = (1 << 2),	/* Don't do command substitution.  */
	WRDE_REUSE = (1 << 3),	/* Reuse storage in PWORDEXP.  */
	WRDE_SHOWERR = (1 << 4),	/* Don't redirect stderr to /dev/null.  */
	WRDE_UNDEF = (1 << 5),	/* Error for expanding undefined variables.  */
	__WRDE_FLAGS = (WRDE_DOOFFS | WRDE_APPEND | WRDE_NOCMD |
	                WRDE_REUSE | WRDE_SHOWERR | WRDE_UNDEF)
};

/* Structure describing a word-expansion run.  */
typedef struct {
	size_t we_wordc;		/* Count of words matched.  */
	char **we_wordv;		/* List of expanded words.  */
	size_t we_offs;		/* Slots to reserve in `we_wordv'.  */
} wordexp_t;

/* Possible nonzero return values from `wordexp'.  */
enum {
#ifdef __USE_XOPEN
	WRDE_NOSYS = -1,		/* Never used since we support `wordexp'.  */
#endif
	WRDE_NOSPACE = 1,		/* Ran out of memory.  */
	WRDE_BADCHAR,		/* A metachar appears in the wrong place.  */
	WRDE_BADVAL,		/* Undefined var reference with WRDE_UNDEF.  */
	WRDE_CMDSUB,		/* Command substitution with WRDE_NOCMD.  */
	WRDE_SYNTAX			/* Shell syntax error.  */
};

/* Do word expansion of WORDS into PWORDEXP.  */
extern int wordexp(__const char *__restrict __words,
                   wordexp_t *__restrict __pwordexp, int __flags) __THROW;

/* Free the storage allocated by a `wordexp' call.  */
extern void wordfree(wordexp_t *__wordexp) __THROW;

__END_DECLS

#endif /* wordexp.h  */
