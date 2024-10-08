#ifndef _RPC_NETDB_H
#include <sunrpc/rpc/netdb.h>

extern int __getrpcbyname_r(__const char *__name, struct rpcent *__result_buf,
                            char *__buffer, size_t __buflen,
                            struct rpcent **__result);
extern int __old_getrpcbyname_r(__const char *__name,
                                struct rpcent *__result_buf,
                                char *__buffer, size_t __buflen,
                                struct rpcent **__result);

extern int __getrpcbynumber_r(int __number, struct rpcent *__result_buf,
                              char *__buffer, size_t __buflen,
                              struct rpcent **__result);
extern int __old_getrpcbynumber_r(int __number, struct rpcent *__result_buf,
                                  char *__buffer, size_t __buflen,
                                  struct rpcent **__result);

extern int __getrpcent_r(struct rpcent *__result_buf, char *__buffer,
                         size_t __buflen, struct rpcent **__result)
attribute_hidden;
extern int __old_getrpcent_r(struct rpcent *__result_buf, char *__buffer,
                             size_t __buflen, struct rpcent **__result);

struct parser_data;
extern int _nss_files_parse_rpcent(char *line, struct rpcent *result,
                                   struct parser_data *data,
                                   size_t datalen, int *errnop);
libc_hidden_proto(_nss_files_parse_rpcent)

#define DECLARE_NSS_PROTOTYPES(service)					      \
extern enum nss_status _nss_ ## service ## _setrpcent (int);		      \
extern enum nss_status _nss_ ## service ## _endrpcent (void);		      \
extern enum nss_status _nss_ ## service ## _getrpcent_r			      \
		       (struct rpcent *rpc, char *buffer, size_t buflen,      \
			int *errnop);					      \
extern enum nss_status _nss_ ## service ## _getrpcbyname_r		      \
		       (const char *name, struct rpcent *rpc,		      \
			char *buffer, size_t buflen, int *errnop);	      \
extern enum nss_status _nss_ ## service ## _getrpcbynumber_r		      \
		       (int number, struct rpcent *rpc,			      \
			char *buffer, size_t buflen, int *errnop);


DECLARE_NSS_PROTOTYPES(files)
DECLARE_NSS_PROTOTYPES(nis)
DECLARE_NSS_PROTOTYPES(nisplus)

#undef DECLARE_NSS_PROTOTYPES

#endif
