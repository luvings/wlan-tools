/* @(#)clnt.h	2.1 88/07/29 4.0 RPCSRC; from 1.31 88/02/08 SMI*/
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

/*
 * clnt.h - Client side remote procedure call interface.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifndef _RPC_CLNT_H
#define _RPC_CLNT_H	1

#include <features.h>
#include <sys/types.h>
#include <rpc/types.h>
#include <rpc/auth.h>
#include <sys/un.h>

__BEGIN_DECLS

/*
 * Rpc calls return an enum clnt_stat.  This should be looked at more,
 * since each implementation is required to live with this (implementation
 * independent) list of errors.
 */
enum clnt_stat {
	RPC_SUCCESS = 0,			/* call succeeded */
	/*
	 * local errors
	 */
	RPC_CANTENCODEARGS = 1,		/* can't encode arguments */
	RPC_CANTDECODERES = 2,		/* can't decode results */
	RPC_CANTSEND = 3,			/* failure in sending call */
	RPC_CANTRECV = 4,			/* failure in receiving result */
	RPC_TIMEDOUT = 5,			/* call timed out */
	/*
	 * remote errors
	 */
	RPC_VERSMISMATCH = 6,		/* rpc versions not compatible */
	RPC_AUTHERROR = 7,		/* authentication error */
	RPC_PROGUNAVAIL = 8,		/* program not available */
	RPC_PROGVERSMISMATCH = 9,		/* program version mismatched */
	RPC_PROCUNAVAIL = 10,		/* procedure unavailable */
	RPC_CANTDECODEARGS = 11,		/* decode arguments error */
	RPC_SYSTEMERROR = 12,		/* generic "other problem" */
	RPC_NOBROADCAST = 21,		/* Broadcasting not supported */
	/*
	 * callrpc & clnt_create errors
	 */
	RPC_UNKNOWNHOST = 13,		/* unknown host name */
	RPC_UNKNOWNPROTO = 17,		/* unknown protocol */
	RPC_UNKNOWNADDR = 19,		/* Remote address unknown */

	/*
	 * rpcbind errors
	 */
	RPC_RPCBFAILURE = 14,		/* portmapper failed in its call */
#define RPC_PMAPFAILURE RPC_RPCBFAILURE
	RPC_PROGNOTREGISTERED = 15,	/* remote program is not registered */
	RPC_N2AXLATEFAILURE = 22,	/* Name to addr translation failed */
	/*
	 * unspecified error
	 */
	RPC_FAILED = 16,
	RPC_INTR = 18,
	RPC_TLIERROR = 20,
	RPC_UDERROR = 23,
	/*
	 * asynchronous errors
	 */
	RPC_INPROGRESS = 24,
	RPC_STALERACHANDLE = 25
};


/*
 * Error info.
 */
struct rpc_err {
	enum clnt_stat re_status;
	union {
		int RE_errno;		/* related system error */
		enum auth_stat RE_why;	/* why the auth error occurred */
		struct {
			u_long low;		/* lowest verion supported */
			u_long high;		/* highest verion supported */
		} RE_vers;

		struct {			/* maybe meaningful if RPC_FAILED */
			long s1;
			long s2;
		} RE_lb;			/* life boot & debugging only */
	} ru;

#define	re_errno	ru.RE_errno
#define	re_why		ru.RE_why
#define	re_vers		ru.RE_vers
#define	re_lb		ru.RE_lb
};


/*
 * Client rpc handle.
 * Created by individual implementations, see e.g. rpc_udp.c.
 * Client is responsible for initializing auth, see e.g. auth_none.c.
 */
typedef struct CLIENT CLIENT;
struct CLIENT {
	AUTH	*cl_auth;		 /* authenticator */
	struct clnt_ops {
		enum clnt_stat(*cl_call)(CLIENT *, u_long, xdrproc_t, caddr_t, xdrproc_t,
		                         caddr_t, struct timeval);
		/* call remote procedure */
		void (*cl_abort)(void);	/* abort a call */
		void (*cl_geterr)(CLIENT *, struct rpc_err *);
		/* get specific error code */
		bool_t (*cl_freeres)(CLIENT *, xdrproc_t, caddr_t);
		/* frees results */
		void (*cl_destroy)(CLIENT *);  /* destroy this structure */
		bool_t (*cl_control)(CLIENT *, int, char *);
		/* the ioctl() of rpc */
	} *cl_ops;

	caddr_t cl_private;		/* private stuff */
};


/*
 * client side rpc interface ops
 *
 * Parameter types are:
 *
 */

/*
 * enum clnt_stat
 * CLNT_CALL(rh, proc, xargs, argsp, xres, resp, timeout)
 * 	CLIENT *rh;
 *	u_long proc;
 *	xdrproc_t xargs;
 *	caddr_t argsp;
 *	xdrproc_t xres;
 *	caddr_t resp;
 *	struct timeval timeout;
 */
#define	CLNT_CALL(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, argsp, xres, resp, secs))
#define	clnt_call(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, argsp, xres, resp, secs))

/*
 * void
 * CLNT_ABORT(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_ABORT(rh)	((*(rh)->cl_ops->cl_abort)(rh))
#define	clnt_abort(rh)	((*(rh)->cl_ops->cl_abort)(rh))

/*
 * struct rpc_err
 * CLNT_GETERR(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_GETERR(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))
#define	clnt_geterr(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))


/*
 * bool_t
 * CLNT_FREERES(rh, xres, resp);
 * 	CLIENT *rh;
 *	xdrproc_t xres;
 *	caddr_t resp;
 */
#define	CLNT_FREERES(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))
#define	clnt_freeres(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))

/*
 * bool_t
 * CLNT_CONTROL(cl, request, info)
 *      CLIENT *cl;
 *      u_int request;
 *      char *info;
 */
#define	CLNT_CONTROL(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))
#define	clnt_control(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))

/*
 * control operations that apply to all transports
 *
 * Note: options marked XXX are no-ops in this implementation of RPC.
 * The are present in TI-RPC but can't be implemented here since they
 * depend on the presence of STREAMS/TLI, which we don't have.
 */
#define CLSET_TIMEOUT        1    /* set timeout (timeval) */
#define CLGET_TIMEOUT        2    /* get timeout (timeval) */
#define CLGET_SERVER_ADDR    3    /* get server's address (sockaddr) */
#define CLGET_FD             6    /* get connections file descriptor */
#define CLGET_SVC_ADDR       7    /* get server's address (netbuf)      XXX */
#define CLSET_FD_CLOSE       8    /* close fd while clnt_destroy */
#define CLSET_FD_NCLOSE      9    /* Do not close fd while clnt_destroy*/
#define CLGET_XID            10   /* Get xid */
#define CLSET_XID            11   /* Set xid */
#define CLGET_VERS           12   /* Get version number */
#define CLSET_VERS           13   /* Set version number */
#define CLGET_PROG           14   /* Get program number */
#define CLSET_PROG           15   /* Set program number */
#define CLSET_SVC_ADDR       16   /* get server's address (netbuf)      XXX */
#define CLSET_PUSH_TIMOD     17   /* push timod if not already present  XXX */
#define CLSET_POP_TIMOD      18   /* pop timod                          XXX */
/*
 * Connectionless only control operations
 */
#define CLSET_RETRY_TIMEOUT	4	/* set retry timeout (timeval) */
#define CLGET_RETRY_TIMEOUT	5	/* get retry timeout (timeval) */

/*
 * void
 * CLNT_DESTROY(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_DESTROY(rh)	((*(rh)->cl_ops->cl_destroy)(rh))
#define	clnt_destroy(rh)	((*(rh)->cl_ops->cl_destroy)(rh))


/*
 * RPCTEST is a test program which is accessible on every rpc
 * transport/port.  It is used for testing, performance evaluation,
 * and network administration.
 */

#define RPCTEST_PROGRAM		((u_long)1)
#define RPCTEST_VERSION		((u_long)1)
#define RPCTEST_NULL_PROC	((u_long)2)
#define RPCTEST_NULL_BATCH_PROC	((u_long)3)

/*
 * By convention, procedure 0 takes null arguments and returns them
 */

#define NULLPROC ((u_long)0)

/*
 * Below are the client handle creation routines for the various
 * implementations of client side rpc.  They can return NULL if a
 * creation failure occurs.
 */

/*
 * Memory based rpc (for speed check and testing)
 * CLIENT *
 * clntraw_create(prog, vers)
 *	u_long prog;
 *	u_long vers;
 */
extern CLIENT *clntraw_create(__const u_long __prog, __const u_long __vers)
__THROW;


/*
 * Generic client creation routine. Supported protocols are "udp", "tcp" and
 * "unix"
 * CLIENT *
 * clnt_create(host, prog, vers, prot)
 *	char *host; 	-- hostname
 *	u_long prog;	-- program number
 *	u_ong vers;	-- version number
 *	char *prot;	-- protocol
 */
extern CLIENT *clnt_create(__const char *__host, __const u_long __prog,
                           __const u_long __vers, __const char *__prot)
__THROW;


/*
 * TCP based rpc
 * CLIENT *
 * clnttcp_create(raddr, prog, vers, sockp, sendsz, recvsz)
 *	struct sockaddr_in *raddr;
 *	u_long prog;
 *	u_long version;
 *	register int *sockp;
 *	u_int sendsz;
 *	u_int recvsz;
 */
extern CLIENT *clnttcp_create(struct sockaddr_in *__raddr, u_long __prog,
                              u_long __version, int *__sockp, u_int __sendsz,
                              u_int __recvsz) __THROW;

/*
 * UDP based rpc.
 * CLIENT *
 * clntudp_create(raddr, program, version, wait, sockp)
 *	struct sockaddr_in *raddr;
 *	u_long program;
 *	u_long version;
 *	struct timeval wait_resend;
 *	int *sockp;
 *
 * Same as above, but you specify max packet sizes.
 * CLIENT *
 * clntudp_bufcreate(raddr, program, version, wait, sockp, sendsz, recvsz)
 *	struct sockaddr_in *raddr;
 *	u_long program;
 *	u_long version;
 *	struct timeval wait_resend;
 *	int *sockp;
 *	u_int sendsz;
 *	u_int recvsz;
 */
extern CLIENT *clntudp_create(struct sockaddr_in *__raddr, u_long __program,
                              u_long __version, struct timeval __wait_resend,
                              int *__sockp) __THROW;
extern CLIENT *clntudp_bufcreate(struct sockaddr_in *__raddr,
                                 u_long __program, u_long __version,
                                 struct timeval __wait_resend, int *__sockp,
                                 u_int __sendsz, u_int __recvsz) __THROW;




/*
 * AF_UNIX based rpc
 * CLIENT *
 * clntunix_create(raddr, prog, vers, sockp, sendsz, recvsz)
 *      struct sockaddr_un *raddr;
 *      u_long prog;
 *      u_long version;
 *      register int *sockp;
 *      u_int sendsz;
 *      u_int recvsz;
 */
extern CLIENT *clntunix_create(struct sockaddr_un *__raddr, u_long __program,
                               u_long __version, int *__sockp,
                               u_int __sendsz, u_int __recvsz) __THROW;


extern int callrpc(__const char *__host, __const u_long __prognum,
                   __const u_long __versnum, __const u_long __procnum,
                   __const xdrproc_t __inproc, __const char *__in,
                   __const xdrproc_t __outproc, char *__out) __THROW;
extern int _rpc_dtablesize(void) __THROW;

/*
 * Print why creation failed
 */
extern void clnt_pcreateerror(__const char *__msg) __THROW;	/* stderr */
extern char *clnt_spcreateerror(__const char *__msg) __THROW;	/* string */

/*
 * Like clnt_perror(), but is more verbose in its output
 */
extern void clnt_perrno(enum clnt_stat __num) __THROW;		/* stderr */

/*
 * Print an English error message, given the client error code
 */
extern void clnt_perror(CLIENT *__clnt, __const char *__msg) __THROW;
/* stderr */
extern char *clnt_sperror(CLIENT *__clnt, __const char *__msg) __THROW;
/* string */

/*
 * If a creation fails, the following allows the user to figure out why.
 */
struct rpc_createerr {
	enum clnt_stat cf_stat;
	struct rpc_err cf_error; /* useful when cf_stat == RPC_PMAPFAILURE */
};

extern struct rpc_createerr rpc_createerr;



/*
 * Copy error message to buffer.
 */
extern char *clnt_sperrno(enum clnt_stat __num) __THROW;	/* string */

/*
 * get the port number on the host for the rpc program,version and proto
 */
extern int getrpcport(__const char * __host, u_long __prognum,
                      u_long __versnum, u_int proto) __THROW;

/*
 * get the local host's IP address without consulting
 * name service library functions
 */
extern void get_myaddress(struct sockaddr_in *) __THROW;

#define UDPMSGSIZE	8800	/* rpc imposed limit on udp msg size */
#define RPCSMALLMSGSIZE	400	/* a more reasonable packet size */

__END_DECLS

#endif /* rpc/clnt.h */
