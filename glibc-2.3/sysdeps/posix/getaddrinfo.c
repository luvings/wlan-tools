/* The Inner Net License, Version 2.00

  The author(s) grant permission for redistribution and use in source and
binary forms, with or without modification, of the software and documentation
provided that the following conditions are met:

0. If you receive a version of the software that is specifically labelled
   as not being for redistribution (check the version message and/or README),
   you are not permitted to redistribute that version of the software in any
   way or form.
1. All terms of the all other applicable copyrights and licenses must be
   followed.
2. Redistributions of source code must retain the authors' copyright
   notice(s), this list of conditions, and the following disclaimer.
3. Redistributions in binary form must reproduce the authors' copyright
   notice(s), this list of conditions, and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
4. [The copyright holder has authorized the removal of this clause.]
5. Neither the name(s) of the author(s) nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ITS AUTHORS AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  If these license terms cause you a real problem, contact the author.  */

/* This software is Copyright 1996 by Craig Metz, All Rights Reserved.  */

#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <net/if.h>

#define GAIH_OKIFUNSPEC 0x0100
#define GAIH_EAI        ~(GAIH_OKIFUNSPEC)

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX  108
#endif

struct gaih_service {
	const char *name;
	int num;
};

struct gaih_servtuple {
	struct gaih_servtuple *next;
	int socktype;
	int protocol;
	int port;
};

static const struct gaih_servtuple nullserv;

struct gaih_addrtuple {
	struct gaih_addrtuple *next;
	int family;
	char addr[16];
	uint32_t scopeid;
};

struct gaih_typeproto {
	int socktype;
	int protocol;
	char name[4];
	int protoflag;
};

/* Values for `protoflag'.  */
#define GAI_PROTO_NOSERVICE	1
#define GAI_PROTO_PROTOANY	2

static const struct gaih_typeproto gaih_inet_typeproto[] = {
	{ 0, 0, "", 0 },
	{ SOCK_STREAM, IPPROTO_TCP, "tcp", 0 },
	{ SOCK_DGRAM, IPPROTO_UDP, "udp", 0 },
	{ SOCK_RAW, 0, "raw", GAI_PROTO_PROTOANY | GAI_PROTO_NOSERVICE },
	{ 0, 0, "", 0 }
};

struct gaih {
	int family;
	int (*gaih)(const char *name, const struct gaih_service *service,
	            const struct addrinfo *req, struct addrinfo **pai);
};

#if PF_UNSPEC == 0
static const struct addrinfo default_hints;
#else
static const struct addrinfo default_hints =
{ 0, PF_UNSPEC, 0, 0, 0, NULL, NULL, NULL };
#endif


#if 0
/* Using Unix sockets this way is a security risk.  */
static int gaih_local(const char *name, const struct gaih_service *service,
                      const struct addrinfo *req, struct addrinfo **pai)
{
	struct utsname utsname;

	if ((name != NULL) && (req->ai_flags & AI_NUMERICHOST))
		return GAIH_OKIFUNSPEC | -EAI_NONAME;

	if ((name != NULL) || (req->ai_flags & AI_CANONNAME))
		if (uname(&utsname) < 0)
			return -EAI_SYSTEM;

	if (name != NULL) {
		if (strcmp(name, "localhost") &&
		                strcmp(name, "local") &&
		                strcmp(name, "unix") &&
		                strcmp(name, utsname.nodename))
			return GAIH_OKIFUNSPEC | -EAI_NONAME;
	}

	if (req->ai_protocol || req->ai_socktype) {
		const struct gaih_typeproto *tp = gaih_inet_typeproto + 1;

		while (tp->name[0]
		                && ((tp->protoflag & GAI_PROTO_NOSERVICE) != 0
		                    || (req->ai_socktype != 0 && req->ai_socktype != tp->socktype)
		                    || (req->ai_protocol != 0
		                        && !(tp->protoflag & GAI_PROTO_PROTOANY)
		                        && req->ai_protocol != tp->protocol)))
			++tp;

		if (! tp->name[0]) {
			if (req->ai_socktype)
				return (GAIH_OKIFUNSPEC | -EAI_SOCKTYPE);

			else
				return (GAIH_OKIFUNSPEC | -EAI_SERVICE);
		}
	}

	*pai = malloc(sizeof(struct addrinfo) + sizeof(struct sockaddr_un)
	              + ((req->ai_flags & AI_CANONNAME)
	                 ? (strlen(utsname.nodename) + 1) : 0));

	if (*pai == NULL)
		return -EAI_MEMORY;

	(*pai)->ai_next = NULL;
	(*pai)->ai_flags = req->ai_flags;
	(*pai)->ai_family = AF_LOCAL;
	(*pai)->ai_socktype = req->ai_socktype ? req->ai_socktype : SOCK_STREAM;
	(*pai)->ai_protocol = req->ai_protocol;
	(*pai)->ai_addrlen = sizeof(struct sockaddr_un);
	(*pai)->ai_addr = (void *)(*pai) + sizeof(struct addrinfo);

#if SALEN
	((struct sockaddr_un *)(*pai)->ai_addr)->sun_len =
	        sizeof(struct sockaddr_un);
#endif /* SALEN */

	((struct sockaddr_un *)(*pai)->ai_addr)->sun_family = AF_LOCAL;
	memset(((struct sockaddr_un *)(*pai)->ai_addr)->sun_path, 0, UNIX_PATH_MAX);

	if (service) {
		struct sockaddr_un *sunp = (struct sockaddr_un *)(*pai)->ai_addr;

		if (strchr(service->name, '/') != NULL) {
			if (strlen(service->name) >= sizeof(sunp->sun_path))
				return GAIH_OKIFUNSPEC | -EAI_SERVICE;

			strcpy(sunp->sun_path, service->name);

		} else {
			if (strlen(P_tmpdir "/") + 1 + strlen(service->name) >=
			                sizeof(sunp->sun_path))
				return GAIH_OKIFUNSPEC | -EAI_SERVICE;

			__stpcpy(__stpcpy(sunp->sun_path, P_tmpdir "/"), service->name);
		}

	} else {
		/* This is a dangerous use of the interface since there is a time
		 window between the test for the file and the actual creation
		 (done by the caller) in which a file with the same name could
		 be created.  */
		char *buf = ((struct sockaddr_un *)(*pai)->ai_addr)->sun_path;

		if (__builtin_expect(__path_search(buf, L_tmpnam, NULL, NULL, 0),
		                     0) != 0
		                || __builtin_expect(__gen_tempname(buf, __GT_NOCREATE), 0) != 0)
			return -EAI_SYSTEM;
	}

	if (req->ai_flags & AI_CANONNAME)
		(*pai)->ai_canonname = strcpy((char *) *pai + sizeof(struct addrinfo)
		                              + sizeof(struct sockaddr_un),
		                              utsname.nodename);

	else
		(*pai)->ai_canonname = NULL;

	return 0;
}

#endif	/* 0 */

static int gaih_inet_serv(const char *servicename, const struct gaih_typeproto *tp,
                          const struct addrinfo *req, struct gaih_servtuple *st)
{
	struct servent *s;
	size_t tmpbuflen = 1024;
	struct servent ts;
	char *tmpbuf;
	int r;

	do {
		tmpbuf = __alloca(tmpbuflen);

		r = __getservbyname_r(servicename, tp->name, &ts, tmpbuf, tmpbuflen,
		                      &s);

		if (r != 0 || s == NULL) {
			if (r == ERANGE)
				tmpbuflen *= 2;

			else
				return GAIH_OKIFUNSPEC | -EAI_SERVICE;
		}
	} while (r);

	st->next = NULL;
	st->socktype = tp->socktype;
	st->protocol = ((tp->protoflag & GAI_PROTO_PROTOANY)
	                ? req->ai_protocol : tp->protocol);
	st->port = s->s_port;

	return 0;
}

#define gethosts(_family, _type)				\
 {								\
  int i, herrno;						\
  size_t tmpbuflen;						\
  struct hostent th;						\
  char *tmpbuf;							\
  tmpbuflen = 512;						\
  no_data = 0;							\
  do {								\
    tmpbuflen *= 2;						\
    tmpbuf = __alloca (tmpbuflen);				\
    rc = __gethostbyname2_r (name, _family, &th, tmpbuf,	\
         tmpbuflen, &h, &herrno);				\
  } while (rc == ERANGE && herrno == NETDB_INTERNAL);		\
  if (rc != 0)							\
    {								\
      if (herrno == NETDB_INTERNAL)				\
	{							\
	  __set_h_errno (herrno);				\
	  return -EAI_SYSTEM;					\
	}							\
      if (herrno == TRY_AGAIN)					\
	no_data = EAI_AGAIN;					\
      else							\
	no_data = herrno == NO_DATA;				\
    }								\
  else if (h != NULL)						\
    {								\
      for (i = 0; h->h_addr_list[i]; i++)			\
	{							\
	  if (*pat == NULL) {					\
	    *pat = __alloca (sizeof(struct gaih_addrtuple));	\
	    (*pat)->scopeid = 0;				\
	  }							\
	  (*pat)->next = NULL;					\
	  (*pat)->family = _family;				\
	  memcpy ((*pat)->addr, h->h_addr_list[i],		\
		 sizeof(_type));				\
	  pat = &((*pat)->next);				\
	}							\
    }								\
 }

static int gaih_inet(const char *name, const struct gaih_service *service,
                     const struct addrinfo *req, struct addrinfo **pai)
{
	const struct gaih_typeproto *tp = gaih_inet_typeproto;
	struct gaih_servtuple *st = (struct gaih_servtuple *) &nullserv;
	struct gaih_addrtuple *at = NULL;
	int rc;

	if (req->ai_protocol || req->ai_socktype) {
		++tp;

		while (tp->name[0]
		                && ((req->ai_socktype != 0 && req->ai_socktype != tp->socktype)
		                    || (req->ai_protocol != 0
		                        && !(tp->protoflag & GAI_PROTO_PROTOANY)
		                        && req->ai_protocol != tp->protocol)))
			++tp;

		if (! tp->name[0]) {
			if (req->ai_socktype)
				return (GAIH_OKIFUNSPEC | -EAI_SOCKTYPE);

			else
				return (GAIH_OKIFUNSPEC | -EAI_SERVICE);
		}
	}

	if (service != NULL) {
		if ((tp->protoflag & GAI_PROTO_NOSERVICE) != 0)
			return (GAIH_OKIFUNSPEC | -EAI_SERVICE);

		if (service->num < 0) {
			if (tp->name[0]) {
				st = (struct gaih_servtuple *)
				     __alloca(sizeof(struct gaih_servtuple));

				if ((rc = gaih_inet_serv(service->name, tp, req, st)))
					return rc;

			} else {
				struct gaih_servtuple **pst = &st;

				for (tp++; tp->name[0]; tp++) {
					struct gaih_servtuple *newp;

					if ((tp->protoflag & GAI_PROTO_NOSERVICE) != 0)
						continue;

					if (req->ai_socktype != 0
					                && req->ai_socktype != tp->socktype)
						continue;

					if (req->ai_protocol != 0
					                && !(tp->protoflag & GAI_PROTO_PROTOANY)
					                && req->ai_protocol != tp->protocol)
						continue;

					newp = (struct gaih_servtuple *)
					       __alloca(sizeof(struct gaih_servtuple));

					if ((rc = gaih_inet_serv(service->name, tp, req, newp))) {
						if (rc & GAIH_OKIFUNSPEC)
							continue;

						return rc;
					}

					*pst = newp;
					pst = &(newp->next);
				}

				if (st == (struct gaih_servtuple *) &nullserv)
					return (GAIH_OKIFUNSPEC | -EAI_SERVICE);
			}

		} else {
			st = __alloca(sizeof(struct gaih_servtuple));
			st->next = NULL;
			st->socktype = tp->socktype;
			st->protocol = ((tp->protoflag & GAI_PROTO_PROTOANY)
			                ? req->ai_protocol : tp->protocol);
			st->port = htons(service->num);
		}

	} else if (req->ai_socktype || req->ai_protocol) {
		st = __alloca(sizeof(struct gaih_servtuple));
		st->next = NULL;
		st->socktype = tp->socktype;
		st->protocol = ((tp->protoflag & GAI_PROTO_PROTOANY)
		                ? req->ai_protocol : tp->protocol);
		st->port = 0;

	} else {
		/* Neither socket type nor protocol is set.  Return all socket types
		 we know about.  */
		struct gaih_servtuple **lastp = &st;

		for (++tp; tp->name[0]; ++tp) {
			struct gaih_servtuple *newp;

			newp = __alloca(sizeof(struct gaih_servtuple));
			newp->next = NULL;
			newp->socktype = tp->socktype;
			newp->protocol = tp->protocol;
			newp->port = 0;

			*lastp = newp;
			lastp = &newp->next;
		}
	}

	if (name != NULL) {
		at = __alloca(sizeof(struct gaih_addrtuple));

		at->family = AF_UNSPEC;
		at->scopeid = 0;
		at->next = NULL;

		if (inet_pton(AF_INET, name, at->addr) > 0) {
			if (req->ai_family == AF_UNSPEC || req->ai_family == AF_INET)
				at->family = AF_INET;

			else
				return -EAI_ADDRFAMILY;
		}

		if (at->family == AF_UNSPEC) {
			char *namebuf = strdupa(name);
			char *scope_delim;

			scope_delim = strchr(namebuf, SCOPE_DELIMITER);

			if (scope_delim != NULL)
				*scope_delim = '\0';

			if (inet_pton(AF_INET6, namebuf, at->addr) > 0) {
				if (req->ai_family == AF_UNSPEC || req->ai_family == AF_INET6)
					at->family = AF_INET6;

				else
					return -EAI_ADDRFAMILY;

				if (scope_delim != NULL) {
					int try_numericscope = 0;

					if (IN6_IS_ADDR_LINKLOCAL(at->addr)
					                || IN6_IS_ADDR_MC_LINKLOCAL(at->addr)) {
						at->scopeid = if_nametoindex(scope_delim + 1);

						if (at->scopeid == 0)
							try_numericscope = 1;

					} else
						try_numericscope = 1;

					if (try_numericscope != 0) {
						char *end;
						assert(sizeof(uint32_t) <= sizeof(unsigned long));
						at->scopeid = (uint32_t) strtoul(scope_delim + 1, &end,
						                                 10);

						if (*end != '\0')
							return GAIH_OKIFUNSPEC | -EAI_NONAME;
					}
				}
			}
		}

		if (at->family == AF_UNSPEC && (req->ai_flags & AI_NUMERICHOST) == 0) {
			struct hostent *h;
			struct gaih_addrtuple **pat = &at;
			int no_data = 0;
			int no_inet6_data;
			int old_res_options = _res.options;

			/* If we are looking for both IPv4 and IPv6 address we don't
			   want the lookup functions to automatically promote IPv4
			   addresses to IPv6 addresses.  Currently this is decided
			   by setting the RES_USE_INET6 bit in _res.options.  */
			if (req->ai_family == AF_UNSPEC)
				_res.options &= ~RES_USE_INET6;

			if (req->ai_family == AF_UNSPEC || req->ai_family == AF_INET6)
				gethosts(AF_INET6, struct in6_addr);

			no_inet6_data = no_data;

			if (req->ai_family == AF_UNSPEC)
				_res.options = old_res_options;

			if (req->ai_family == AF_UNSPEC || req->ai_family == AF_INET)
				gethosts(AF_INET, struct in_addr);

			if (no_data != 0 && no_inet6_data != 0) {
				/* If both requests timed out report this.  */
				if (no_data == EAI_AGAIN && no_inet6_data == EAI_AGAIN)
					return -EAI_AGAIN;

				/* We made requests but they turned out no data.  The name
				 is known, though.  */
				return (GAIH_OKIFUNSPEC | -EAI_NODATA);
			}
		}

		if (at->family == AF_UNSPEC)
			return (GAIH_OKIFUNSPEC | -EAI_NONAME);

	} else {
		struct gaih_addrtuple *atr;
		atr = at = __alloca(sizeof(struct gaih_addrtuple));
		memset(at, '\0', sizeof(struct gaih_addrtuple));

		if (req->ai_family == 0) {
			at->next = __alloca(sizeof(struct gaih_addrtuple));
			memset(at->next, '\0', sizeof(struct gaih_addrtuple));
		}

		if (req->ai_family == 0 || req->ai_family == AF_INET6) {
			at->family = AF_INET6;

			if ((req->ai_flags & AI_PASSIVE) == 0)
				memcpy(at->addr, &in6addr_loopback, sizeof(struct in6_addr));

			atr = at->next;
		}

		if (req->ai_family == 0 || req->ai_family == AF_INET) {
			atr->family = AF_INET;

			if ((req->ai_flags & AI_PASSIVE) == 0)
				*(uint32_t *) atr->addr = htonl(INADDR_LOOPBACK);
		}
	}

	if (pai == NULL)
		return 0;

	{
		const char *c = NULL;
		struct gaih_servtuple *st2;
		struct gaih_addrtuple *at2 = at;
		size_t socklen, namelen;
		sa_family_t family;

		/*
		  buffer is the size of an unformatted IPv6 address in printable format.
		 */
		while (at2 != NULL) {
			if (req->ai_flags & AI_CANONNAME) {
				struct hostent *h = NULL;

				int herrno;
				struct hostent th;
				size_t tmpbuflen = 512;
				char *tmpbuf;

				do {
					tmpbuflen *= 2;
					tmpbuf = __alloca(tmpbuflen);

					rc = __gethostbyaddr_r(at2->addr,
					                       ((at2->family == AF_INET6)
					                        ? sizeof(struct in6_addr)
					                        : sizeof(struct in_addr)),
					                       at2->family, &th, tmpbuf, tmpbuflen,
					                       &h, &herrno);

				} while (rc == errno && herrno == NETDB_INTERNAL);

				if (rc != 0 && herrno == NETDB_INTERNAL) {
					__set_h_errno(herrno);
					return -EAI_SYSTEM;
				}

				if (h != NULL)
					c = h->h_name;

				else {
					/* We have to try to get the canonical in some other
					   way.  If we are looking for either AF_INET or
					   AF_INET6 try the other line.  */
					if (req->ai_family == AF_UNSPEC) {
						struct addrinfo *p = NULL;
						struct addrinfo **end = &p;
						struct addrinfo localreq = *req;
						struct addrinfo *runp;

						localreq.ai_family = AF_INET + AF_INET6 - at2->family;
						(void) gaih_inet(name, service, &localreq, end);

						runp = p;

						while (runp != NULL) {
							if (p->ai_canonname != name) {
								c = strdupa(p->ai_canonname);
								break;
							}

							runp = runp->ai_next;
						}

						freeaddrinfo(p);
					}

					/* If this code is used the missing canonical name is
					   substituted with the name passed in by the user.  */
					if (c == NULL)
						c = name;
				}

				if (c == NULL)
					return GAIH_OKIFUNSPEC | -EAI_NONAME;

				namelen = strlen(c) + 1;

			} else
				namelen = 0;

			if (at2->family == AF_INET6) {
				family = AF_INET6;
				socklen = sizeof(struct sockaddr_in6);

			} else {
				family = AF_INET;
				socklen = sizeof(struct sockaddr_in);
			}

			for (st2 = st; st2 != NULL; st2 = st2->next) {
				*pai = malloc(sizeof(struct addrinfo) + socklen + namelen);

				if (*pai == NULL)
					return -EAI_MEMORY;

				(*pai)->ai_flags = req->ai_flags;
				(*pai)->ai_family = family;
				(*pai)->ai_socktype = st2->socktype;
				(*pai)->ai_protocol = st2->protocol;
				(*pai)->ai_addrlen = socklen;
				(*pai)->ai_addr = (void *)(*pai) + sizeof(struct addrinfo);
#if SALEN
				(*pai)->ai_addr->sa_len = socklen;
#endif /* SALEN */
				(*pai)->ai_addr->sa_family = family;

				if (family == AF_INET6) {
					struct sockaddr_in6 *sin6p =
					        (struct sockaddr_in6 *)(*pai)->ai_addr;

					sin6p->sin6_flowinfo = 0;
					memcpy(&sin6p->sin6_addr,
					       at2->addr, sizeof(struct in6_addr));
					sin6p->sin6_port = st2->port;
					sin6p->sin6_scope_id = at2->scopeid;

				} else {
					struct sockaddr_in *sinp =
					        (struct sockaddr_in *)(*pai)->ai_addr;
					memcpy(&sinp->sin_addr,
					       at2->addr, sizeof(struct in_addr));
					sinp->sin_port = st2->port;
					memset(sinp->sin_zero, '\0', sizeof(sinp->sin_zero));
				}

				if (c) {
					(*pai)->ai_canonname = ((void *)(*pai) +
					                        sizeof(struct addrinfo) + socklen);
					strcpy((*pai)->ai_canonname, c);

				} else
					(*pai)->ai_canonname = NULL;

				(*pai)->ai_next = NULL;
				pai = &((*pai)->ai_next);
			}

			at2 = at2->next;
		}
	}

	return 0;
}

static struct gaih gaih[] = {
	{ PF_INET6, gaih_inet },
	{ PF_INET, gaih_inet },
#if 0
	{ PF_LOCAL, gaih_local },
#endif
	{ PF_UNSPEC, NULL }
};

int getaddrinfo(const char *name, const char *service,
                const struct addrinfo *hints, struct addrinfo **pai)
{
	int i = 0, j = 0, last_i = 0;
	struct addrinfo *p = NULL, **end;
	struct gaih *g = gaih, *pg = NULL;
	struct gaih_service gaih_service, *pservice;

	if (name != NULL && name[0] == '*' && name[1] == 0)
		name = NULL;

	if (service != NULL && service[0] == '*' && service[1] == 0)
		service = NULL;

	if (name == NULL && service == NULL)
		return EAI_NONAME;

	if (hints == NULL)
		hints = &default_hints;

	if (hints->ai_flags & ~(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST))
		return EAI_BADFLAGS;

	if ((hints->ai_flags & AI_CANONNAME) && name == NULL)
		return EAI_BADFLAGS;

	if (service && service[0]) {
		char *c;
		gaih_service.name = service;
		gaih_service.num = strtoul(gaih_service.name, &c, 10);

		if (*c)
			gaih_service.num = -1;

		else

			/* Can't specify a numerical socket unless a protocol family was
			   given. */
			if (hints->ai_socktype == 0 && hints->ai_protocol == 0)
				return EAI_SERVICE;

		pservice = &gaih_service;

	} else
		pservice = NULL;

	if (pai)
		end = &p;

	else
		end = NULL;

	while (g->gaih) {
		if (hints->ai_family == g->family || hints->ai_family == AF_UNSPEC) {
			j++;

			if (pg == NULL || pg->gaih != g->gaih) {
				pg = g;
				i = g->gaih(name, pservice, hints, end);

				if (i != 0) {
					/* EAI_NODATA is a more specific result as it says that
					   we found a result but it is not usable.  */
					if (last_i != (GAIH_OKIFUNSPEC | -EAI_NODATA))
						last_i = i;

					if (hints->ai_family == AF_UNSPEC && (i & GAIH_OKIFUNSPEC)) {
						++g;
						continue;
					}

					freeaddrinfo(p);

					return -(i & GAIH_EAI);
				}

				if (end)
					while (*end) end = &((*end)->ai_next);
			}
		}

		++g;
	}

	if (j == 0)
		return EAI_FAMILY;

	if (p) {
		*pai = p;
		return 0;
	}

	if (pai == NULL && last_i == 0)
		return 0;

	freeaddrinfo(p);

	return last_i ? -(last_i & GAIH_EAI) : EAI_NONAME;
}

libc_hidden_def(getaddrinfo)

void
freeaddrinfo(struct addrinfo *ai)
{
	struct addrinfo *p;

	while (ai != NULL) {
		p = ai;
		ai = ai->ai_next;
		free(p);
	}
}

libc_hidden_def(freeaddrinfo)
