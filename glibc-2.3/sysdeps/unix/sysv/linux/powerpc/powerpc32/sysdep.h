/* Copyright (C) 1992,97,98,99,2000,01,02 Free Software Foundation, Inc.
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

#ifndef _LINUX_POWERPC_SYSDEP_H
#define _LINUX_POWERPC_SYSDEP_H 1

#include <sysdeps/unix/powerpc/sysdep.h>

/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#ifdef __STDC__
# define SYS_ify(syscall_name)	__NR_##syscall_name
#else
# define SYS_ify(syscall_name)	__NR_/**/syscall_name
#endif

#ifndef __ASSEMBLER__

# include <errno.h>

# undef INLINE_SYSCALL
# define INLINE_SYSCALL(name, nr, args...)	\
  ({						\
    long ret, err;				\
    LOADARGS_##nr(name, args);			\
    __asm __volatile ("sc\n\t"			\
		      "mfcr	%1\n\t"		\
		      : "=r" (r3), "=r" (err)	\
		      : ASM_INPUT_##nr		\
		      : "cc", "memory");	\
    ret = r3;					\
    if (err & 1 << 28)				\
      {						\
	__set_errno (ret);			\
	ret = -1L;				\
      }						\
    ret;					\
  })

# define LOADARGS_0(name) \
	register long r0 __asm__ ("r0") = __NR_##name; \
	register long r3 __asm__ ("r3")
# define LOADARGS_1(name, arg1) \
	LOADARGS_0(name) = (long) (arg1)
# define LOADARGS_2(name, arg1, arg2) \
	LOADARGS_1(name, arg1); \
	register long r4 __asm__ ("r4") = (long) (arg2)
# define LOADARGS_3(name, arg1, arg2, arg3) \
	LOADARGS_2(name, arg1, arg2); \
	register long r5 __asm__ ("r5") = (long) (arg3)
# define LOADARGS_4(name, arg1, arg2, arg3, arg4) \
	LOADARGS_3(name, arg1, arg2, arg3); \
	register long r6 __asm__ ("r6") = (long) (arg4)
# define LOADARGS_5(name, arg1, arg2, arg3, arg4, arg5) \
	LOADARGS_4(name, arg1, arg2, arg3, arg4); \
	register long r7 __asm__ ("r7") = (long) (arg5)
# define LOADARGS_6(name, arg1, arg2, arg3, arg4, arg5, arg6) \
	LOADARGS_5(name, arg1, arg2, arg3, arg4, arg5); \
	register long r8 __asm__ ("r8") = (long) (arg6)

# define ASM_INPUT_0 "r" (r0)
# define ASM_INPUT_1 ASM_INPUT_0, "0" (r3)
# define ASM_INPUT_2 ASM_INPUT_1, "r" (r4)
# define ASM_INPUT_3 ASM_INPUT_2, "r" (r5)
# define ASM_INPUT_4 ASM_INPUT_3, "r" (r6)
# define ASM_INPUT_5 ASM_INPUT_4, "r" (r7)
# define ASM_INPUT_6 ASM_INPUT_5, "r" (r8)

#endif /* __ASSEMBLER__ */


#endif /* linux/powerpc/sysdep.h */
