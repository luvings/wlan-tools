/*
 * netlink-local.h		Local Netlink Interface
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation version 2.1
 *	of the License.
 *
 * Copyright (c) 2003-2008 Thomas Graf <tgraf@suug.ch>
 */

#ifndef NETLINK_LOCAL_H_
#define NETLINK_LOCAL_H_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <inttypes.h>
#include <assert.h>
#include <limits.h>
#include <search.h>

#include <arpa/inet.h>
#include <netdb.h>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#include <linux/types.h>

/* local header copies */
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/pkt_sched.h>
#include <linux/pkt_cls.h>
#include <linux/gen_stats.h>
#include <linux/ip_mp_alg.h>
#include <linux/atm.h>
#include <linux/inetdevice.h>
#include <linux/ipv6.h>
#include <linux/snmp.h>

#include <netlink/netlink.h>
#include <netlink/handlers.h>
#include <netlink/cache.h>
#include <netlink/route/tc.h>
#include <netlink/object-api.h>
#include <netlink/cache-api.h>
#include <netlink-types.h>

struct trans_tbl {
	int i;
	const char *a;
};

#define __ADD(id, name) { .i = id, .a = #name },

struct trans_list {
	int i;
	char *a;
	struct nl_list_head list;
};

#define NL_DEBUG	1

#define NL_DBG(LVL,FMT,ARG...) \
	do {	\
		if (LVL <= nl_debug) \
			fprintf(stderr, "DBG<" #LVL ">: " FMT, ##ARG); \
	} while (0)

#define BUG()                            \
	do {                                 \
		fprintf(stderr, "BUG: %s:%d\n",  \
			__FILE__, __LINE__);         \
		assert(0);	\
	} while (0)

extern int __nl_read_num_str_file(const char *path,
                                  int (*cb)(long, const char *));

extern int __trans_list_add(int, const char *, struct nl_list_head *);
extern void __trans_list_clear(struct nl_list_head *);

extern char *__type2str(int, char *, size_t, const struct trans_tbl *, size_t);
extern int __str2type(const char *, const struct trans_tbl *, size_t);

extern char *__list_type2str(int, char *, size_t, struct nl_list_head *);
extern int __list_str2type(const char *, struct nl_list_head *);

extern char *__flags2str(int, char *, size_t, const struct trans_tbl *, size_t);
extern int __str2flags(const char *, const struct trans_tbl *, size_t);

extern void dump_from_ops(struct nl_object *, struct nl_dump_params *);

static inline struct nl_cache *dp_cache(struct nl_object *obj)
{
	if (obj->ce_cache == NULL)
		return nl_cache_mngt_require(obj->ce_ops->oo_name);

	return obj->ce_cache;
}

static inline int nl_cb_call(struct nl_cb *cb, int type, struct nl_msg *msg)
{
	return cb->cb_set[type](msg, cb->cb_args[type]);
}

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define __init __attribute__ ((constructor))
#define __exit __attribute__ ((destructor))
#undef __deprecated
#define __deprecated __attribute__ ((deprecated))

#define min(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x < _y ? _x : _y; })

#define max(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x > _y ? _x : _y; })

#define min_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

extern int nl_cache_parse(struct nl_cache_ops *, struct sockaddr_nl *,
                          struct nlmsghdr *, struct nl_parser_param *);


static inline void rtnl_copy_ratespec(struct rtnl_ratespec *dst,
                                      struct tc_ratespec *src)
{
	dst->rs_cell_log = src->cell_log;
	dst->rs_overhead = src->overhead;
	dst->rs_cell_align = src->cell_align;
	dst->rs_mpu = src->mpu;
	dst->rs_rate = src->rate;
}

static inline void rtnl_rcopy_ratespec(struct tc_ratespec *dst,
                                       struct rtnl_ratespec *src)
{
	dst->cell_log = src->rs_cell_log;
	dst->overhead = src->rs_overhead;
	dst->cell_align = src->rs_cell_align;
	dst->mpu = src->rs_mpu;
	dst->rate = src->rs_rate;
}

static inline char *nl_cache_name(struct nl_cache *cache)
{
	return cache->c_ops ? cache->c_ops->co_name : "unknown";
}

#define GENL_FAMILY(id, name) \
	{ \
		{ id, NL_ACT_UNSPEC, name }, \
		END_OF_MSGTYPES_LIST, \
	}

static inline int wait_for_ack(struct nl_sock *sk)
{
	if (sk->s_flags & NL_NO_AUTO_ACK)
		return 0;

	else
		return nl_wait_for_ack(sk);
}

#endif
