#ifndef _FCNTL_H
#include <io/fcntl.h>

/* Now define the internal interfaces.  */
extern int __open64(__const char *__file, int __oflag, ...);
libc_hidden_proto(__open64)
extern int __libc_open64(const char *file, int oflag, ...);
extern int __libc_open(const char *file, int oflag, ...);
libc_hidden_proto(__libc_open)
extern int __libc_fcntl(int fd, int cmd, ...);
libc_hidden_proto(__libc_fcntl)
extern int __open(__const char *__file, int __oflag, ...);
libc_hidden_proto(__open)
extern int __fcntl(int __fd, int __cmd, ...);
libc_hidden_proto(__fcntl)

#endif
