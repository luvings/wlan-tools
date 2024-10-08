/* Copyright (C) 1992,95,97,99,2000 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Brendan Kehoe (brendan@zen.org).

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

#include <sysdeps/unix/sysdep.h>

#ifdef __ASSEMBLER__

#include <regdef.h>

#define ENTRY(name) \
  .globl name;								      \
  .align 2;								      \
  .ent name,0;								      \
  name##:

/* Note that while it's better structurally, going back to call __syscall_error
   can make things confusing if you're debugging---it looks like it's jumping
   backwards into the previous fn.  */
#ifdef __PIC__
#define PSEUDO(name, syscall_name, args) \
  .align 2;								      \
  99: la t9,__syscall_error;						      \
  jr t9;								      \
  ENTRY(name)								      \
  .set noreorder;							      \
  .cpload t9;								      \
  li v0, SYS_ify(syscall_name);						      \
  syscall;								      \
  .set reorder;								      \
  bne a3, zero, 99b;							      \
syse1:
#else
#define PSEUDO(name, syscall_name, args) \
  .set noreorder;							      \
  .align 2;								      \
  99: j __syscall_error;							      \
  ENTRY(name)								      \
  .set noreorder;							      \
  li v0, SYS_ify(syscall_name);						      \
  syscall;								      \
  .set reorder;								      \
  bne a3, zero, 99b;							      \
syse1:
#endif

#undef PSEUDO_END
#define PSEUDO_END(sym) .end sym

#define ret	j ra ; nop

#undef END
#define END(sym)        .end sym

#define r0	v0
#define r1	v1
/* The mips move insn is d,s.  */
#define MOVE(x,y)	move y , x

#endif
