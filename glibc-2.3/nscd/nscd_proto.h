/* Copyright (C) 1998, 1999, 2000, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Thorsten Kukuk <kukuk@suse.de>, 1998.

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

#ifndef _NSCD_PROTO_H
#define _NSCD_PROTO_H 1

#include <grp.h>
#include <netdb.h>
#include <pwd.h>

/* Variables for communication between NSCD handler functions and NSS.  */
extern int __nss_not_use_nscd_passwd attribute_hidden;
extern int __nss_not_use_nscd_group attribute_hidden;
extern int __nss_not_use_nscd_hosts attribute_hidden;

extern int __nscd_getpwnam_r(const char *name, struct passwd *resultbuf,
                             char *buffer, size_t buflen);
extern int __nscd_getpwuid_r(uid_t uid, struct passwd *resultbuf,
                             char *buffer,  size_t buflen);
extern int __nscd_getgrnam_r(const char *name, struct group *resultbuf,
                             char *buffer, size_t buflen);
extern int __nscd_getgrgid_r(uid_t uid, struct group *resultbuf,
                             char *buffer,  size_t buflen);
extern int __nscd_gethostbyname_r(const char *name,
                                  struct hostent *resultbuf,
                                  char *buffer, size_t buflen,
                                  int *h_errnop);
extern int __nscd_gethostbyname2_r(const char *name, int af,
                                   struct hostent *resultbuf,
                                   char *buffer, size_t buflen,
                                   int *h_errnop);
extern int __nscd_gethostbyaddr_r(const void *addr, socklen_t len, int type,
                                  struct hostent *resultbuf,
                                  char *buffer, size_t buflen,
                                  int *h_errnop);

#endif /* _NSCD_PROTO_H */
