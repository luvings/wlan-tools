/* Copyright (C) 1997, 1998, 1999, 2000 Free Software Foundation, Inc.
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

#ifndef _ICONV_H
#define _ICONV_H	1

#include <features.h>
#define __need_size_t
#include <stddef.h>


__BEGIN_DECLS

/* Identifier for conversion method from one codeset to another.  */
typedef void *iconv_t;


/* Allocate descriptor for code conversion from codeset FROMCODE to
   codeset TOCODE.  */
extern iconv_t iconv_open(__const char *__tocode, __const char *__fromcode)
__THROW;

/* Convert at most *INBYTESLEFT bytes from *INBUF according to the
   code conversion algorithm specified by CD and place up to
   *OUTBYTESLEFT bytes in buffer at *OUTBUF.  */
extern size_t iconv(iconv_t __cd, char **__restrict __inbuf,
                    size_t *__restrict __inbytesleft,
                    char **__restrict __outbuf,
                    size_t *__restrict __outbytesleft);

/* Free resources allocated for descriptor CD for code conversion.  */
extern int iconv_close(iconv_t __cd) __THROW;

__END_DECLS

#endif /* iconv.h */
