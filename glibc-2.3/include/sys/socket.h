#ifndef _SYS_SOCKET_H
#include <socket/sys/socket.h>

/* Now define the internal interfaces.  */

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
extern int __socket(int __domain, int __type,
                    int __protocol) attribute_hidden;

/* Create two new sockets, of type TYPE in domain DOMAIN and using
   protocol PROTOCOL, which are connected to each other, and put file
   descriptors for them in FDS[0] and FDS[1].  If PROTOCOL is zero,
   one will be chosen automatically.  Returns 0 on success, -1 for errors.  */
extern int __socketpair(int __domain, int __type, int __protocol,
                        int __fds[2]) attribute_hidden;

/* Return a socket of any type.  The socket can be used in subsequent
   ioctl calls to talk to the kernel.  */
extern int __opensock(void) internal_function attribute_hidden;

/* Put the address of the peer connected to socket FD into *ADDR
   (which is *LEN bytes long), and its actual length into *LEN.  */
extern int __getpeername(int __fd, __SOCKADDR_ARG __addr,
                         socklen_t *__len) attribute_hidden;

/* Send N bytes of BUF to socket FD.  Returns the number sent or -1.  */
extern ssize_t __libc_send(int __fd, __const void *__buf, size_t __n,
                           int __flags);
extern ssize_t __send(int __fd, __const void *__buf, size_t __n, int __flags);
libc_hidden_proto(__send)

/* Read N bytes into BUF from socket FD.
   Returns the number read or -1 for errors.  */
extern ssize_t __libc_recv(int __fd, void *__buf, size_t __n, int __flags);

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.  */
extern ssize_t __libc_sendto(int __fd, __const void *__buf, size_t __n,
                             int __flags, __CONST_SOCKADDR_ARG __addr,
                             socklen_t __addr_len);

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address of
   the sender, and store the actual size of the address in *ADDR_LEN.
   Returns the number of bytes read or -1 for errors.  */
extern ssize_t __libc_recvfrom(int __fd, void *__restrict __buf, size_t __n,
                               int __flags, __SOCKADDR_ARG __addr,
                               socklen_t *__restrict __addr_len);

/* Open a connection on socket FD to peer at ADDR (which LEN bytes long).
   For connectionless socket types, just set the default address to send to
   and the only address from which to accept transmissions.
   Return 0 on success, -1 for errors.  */
extern int __libc_connect(int __fd, __CONST_SOCKADDR_ARG __addr,
                          socklen_t __len);
extern int __connect(int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
extern int __connect_internal(int __fd, __CONST_SOCKADDR_ARG __addr,
                              socklen_t __len) attribute_hidden;

/* Send N bytes of BUF on socket FD to peer at address ADDR (which is
   ADDR_LEN bytes long).  Returns the number sent, or -1 for errors.  */
extern ssize_t __libc_sendto(int __fd, __const void *__buf, size_t __n,
                             int __flags, __CONST_SOCKADDR_ARG __addr,
                             socklen_t __addr_len);
extern ssize_t __sendto(int __fd, __const void *__buf, size_t __n,
                        int __flags, __CONST_SOCKADDR_ARG __addr,
                        socklen_t __addr_len) attribute_hidden;

/* Read N bytes into BUF through socket FD.
   If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address of
   the sender, and store the actual size of the address in *ADDR_LEN.
   Returns the number of bytes read or -1 for errors.  */
extern ssize_t __recvfrom(int __fd, void *__restrict __buf, size_t __n,
                          int __flags, __SOCKADDR_ARG __addr,
                          socklen_t *__restrict __addr_len) attribute_hidden;

/* Send a message described MESSAGE on socket FD.
   Returns the number of bytes sent, or -1 for errors.  */
extern ssize_t __libc_sendmsg(int __fd, __const struct msghdr *__message,
                              int __flags);
extern ssize_t __sendmsg(int __fd, __const struct msghdr *__message,
                         int __flags) attribute_hidden;

/* Receive a message as described by MESSAGE from socket FD.
   Returns the number of bytes read or -1 for errors.  */
extern ssize_t __libc_recvmsg(int __fd, struct msghdr *__message,
                              int __flags);
extern ssize_t __recvmsg(int __fd, struct msghdr *__message,
                         int __flags) attribute_hidden;

/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
extern int __setsockopt(int __fd, int __level, int __optname,
                        __const void *__optval,
                        socklen_t __optlen) attribute_hidden;

/* Put the local address of FD into *ADDR and its length in *LEN.  */
extern int __getsockname(int __fd, __SOCKADDR_ARG __addr,
                         socklen_t *__restrict __len) attribute_hidden;

/* Give the socket FD the local address ADDR (which is LEN bytes long).  */
extern int __bind(int __fd, __CONST_SOCKADDR_ARG __addr,
                  socklen_t __len) attribute_hidden;

/* Prepare to accept connections on socket FD.
   N connection requests will be queued before further requests are refused.
   Returns 0 on success, -1 for errors.  */
extern int __listen(int __fd, int __n) attribute_hidden;

/* Await a connection on socket FD.
   When a connection arrives, open a new socket to communicate with it,
   set *ADDR (which is *ADDR_LEN bytes long) to the address of the connecting
   peer and *ADDR_LEN to the address's actual length, and return the
   new socket's descriptor, or -1 for errors.  */
extern int __libc_accept(int __fd, __SOCKADDR_ARG __addr,
                         socklen_t *__restrict __addr_len)
__THROW;

/* Return the length of a `sockaddr' structure.  */
#ifdef _HAVE_SA_LEN
# define SA_LEN(_x)      (_x)->sa_len
#else
extern int __libc_sa_len(sa_family_t __af);
extern int __libc_sa_len_internal(sa_family_t __af) attribute_hidden;
# ifndef NOT_IN_libc
#  define SA_LEN(_x)      INTUSE(__libc_sa_len)((_x)->sa_family)
# else
#  define SA_LEN(_x)      __libc_sa_len((_x)->sa_family)
# endif
#endif


#ifndef NOT_IN_libc
# define __connect(fd, addr, len) INTUSE(__connect) (fd, addr, len)
#endif

#endif
