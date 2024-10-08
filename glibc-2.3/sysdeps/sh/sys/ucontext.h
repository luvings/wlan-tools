/* Copyright (C) 1999, 2000, 2001 Free Software Foundation, Inc.
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

/* Where is System V/SH ABI?  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <signal.h>

typedef int greg_t;

/* Number of general registers.  */
#define NFPREG	16

/* Container for all general registers.  */
typedef greg_t gregset_t[NFPREG];

/* Number of each register is the `gregset_t' array.  */
enum {
	R0 = 0,
#define R0	R0
	R1 = 1,
#define R1	R1
	R2 = 2,
#define R2	R2
	R3 = 3,
#define R3	R3
	R4 = 4,
#define R4	R4
	R5 = 5,
#define R5	R5
	R6 = 6,
#define R6	R6
	R7 = 7,
#define R7	R7
	R8 = 8,
#define R8	R8
	R9 = 9,
#define R9	R9
	R10 = 10,
#define R10	R10
	R11 = 11,
#define R11	R11
	R12 = 12,
#define R12	R12
	R13 = 13,
#define R13	R13
	R14 = 14,
#define R14	R14
	R15 = 15,
#define R15	R15
};

typedef int freg_t;

/* Number of FPU registers.  */
#define NFREG	16

/* Structure to describe FPU registers.  */
typedef freg_t fpregset_t[NFREG];

/* Context to describe whole processor state.  */
typedef struct {
	gregset_t gregs;
	fpregset_t fpregs;
	fpregset_t xfpregs;
} mcontext_t;

/* Userlevel context.  */
typedef struct ucontext {
	unsigned long int uc_flags;
	struct ucontext *uc_link;
	__sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	long int uc_filler[5];
} ucontext_t;

#endif /* sys/ucontext.h */
