/* Dump registers.
   Copyright (C) 1998, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Andreas Schwab <schwab@gnu.org>.

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

#include <stddef.h>
#include <sys/uio.h>
#include <stdio-common/_itoa.h>

/* We will print the register dump in this format:

  D0: XXXXXXXX   D1: XXXXXXXX   D2: XXXXXXXX   D3: XXXXXXXX
  D4: XXXXXXXX   D5: XXXXXXXX   D6: XXXXXXXX   D7: XXXXXXXX
  A0: XXXXXXXX   A1: XXXXXXXX   A2: XXXXXXXX   A3: XXXXXXXX
  A4: XXXXXXXX   A5: XXXXXXXX   A6: XXXXXXXX   A7: XXXXXXXX
  PC: XXXXXXXX   SR: XXXX

  OldMask: XXXXXXXX  Vector: XXXX

  FP0: XXXXXXXXXXXXXXXXXXXXXXXX   FP1: XXXXXXXXXXXXXXXXXXXXXXXX
  FP2: XXXXXXXXXXXXXXXXXXXXXXXX   FP3: XXXXXXXXXXXXXXXXXXXXXXXX
  FP4: XXXXXXXXXXXXXXXXXXXXXXXX   FP5: XXXXXXXXXXXXXXXXXXXXXXXX
  FP6: XXXXXXXXXXXXXXXXXXXXXXXX   FP7: XXXXXXXXXXXXXXXXXXXXXXXX
  FPCR: XXXXXXXX   FPSR: XXXXXXXX   FPIAR: XXXXXXXX

*/

/* Linux saves only the call-clobbered registers in the sigcontext.  We
   need to use a trampoline that saves the rest so that the C code can
   access them.  We use the sc_fpstate field, since the handler is not
   supposed to return anyway, thus it doesn't matter that it's clobbered.  */

/* static */ void catch_segfault(int, int, struct sigcontext *);

/* Dummy function so that we can use asm with arguments.  */
static void __attribute__((unused))
__dummy__(void)
{
	asm("\n\
catch_segfault:\n\
	move.l 12(%%sp),%%a0\n\
	lea %c0(%%a0),%%a0\n\
	/* Clear the first 4 bytes to make it a null fp state, just\n\
	   in case the handler does return.  */\n\
	clr.l (%%a0)+\n\
	movem.l %%d2-%%d7/%%a2-%%a6,(%%a0)\n\
	fmovem.x %%fp2-%%fp7,11*4(%%a0)\n\
	jra real_catch_segfault"
	    : : "n"(offsetof(struct sigcontext, sc_fpstate)));
}

#define catch_segfault(a,b) \
  __attribute__ ((unused)) real_catch_segfault(a,b)

static void hexvalue(unsigned long int value, char *buf, size_t len)
{
	char *cp = _itoa_word(value, buf + len, 16, 0);

	while (cp > buf)
		*--cp = '0';
}

static void register_dump(int fd, struct sigcontext *ctx)
{
	char regs[20][8];
	char fpregs[11][24];
	struct iovec iov[63], *next_iov = iov;
	unsigned long *p = (unsigned long *) ctx->sc_fpstate + 1;

#define ADD_STRING(str) \
  next_iov->iov_base = (char *) (str); \
  next_iov->iov_len = strlen (str); \
  ++next_iov
#define ADD_MEM(str, len) \
  next_iov->iov_base = (str); \
  next_iov->iov_len = (len); \
  ++next_iov

	/* Generate strings of register contents.  */
	hexvalue(ctx->sc_d0, regs[0], 8);
	hexvalue(ctx->sc_d1, regs[1], 8);
	hexvalue(*p++, regs[2], 8);
	hexvalue(*p++, regs[3], 8);
	hexvalue(*p++, regs[4], 8);
	hexvalue(*p++, regs[5], 8);
	hexvalue(*p++, regs[6], 8);
	hexvalue(*p++, regs[7], 8);
	hexvalue(ctx->sc_a0, regs[8], 8);
	hexvalue(ctx->sc_a1, regs[9], 8);
	hexvalue(*p++, regs[10], 8);
	hexvalue(*p++, regs[11], 8);
	hexvalue(*p++, regs[12], 8);
	hexvalue(*p++, regs[13], 8);
	hexvalue(*p++, regs[14], 8);
	hexvalue(ctx->sc_usp, regs[15], 8);
	hexvalue(ctx->sc_pc, regs[16], 8);
	hexvalue(ctx->sc_sr, regs[17], 4);
	hexvalue(ctx->sc_mask, regs[18], 8);
	hexvalue(ctx->sc_formatvec & 0xfff, regs[19], 4);
	hexvalue(ctx->sc_fpregs[0], fpregs[0], 8);
	hexvalue(ctx->sc_fpregs[1], fpregs[0] + 8, 8);
	hexvalue(ctx->sc_fpregs[2], fpregs[0] + 16, 8);
	hexvalue(ctx->sc_fpregs[3], fpregs[1], 8);
	hexvalue(ctx->sc_fpregs[4], fpregs[1] + 8, 8);
	hexvalue(ctx->sc_fpregs[5], fpregs[1] + 16, 8);
	hexvalue(*p++, fpregs[2], 8);
	hexvalue(*p++, fpregs[2] + 8, 8);
	hexvalue(*p++, fpregs[2] + 16, 8);
	hexvalue(*p++, fpregs[3], 8);
	hexvalue(*p++, fpregs[3] + 8, 8);
	hexvalue(*p++, fpregs[3] + 16, 8);
	hexvalue(*p++, fpregs[4], 8);
	hexvalue(*p++, fpregs[4] + 8, 8);
	hexvalue(*p++, fpregs[4] + 16, 8);
	hexvalue(*p++, fpregs[5], 8);
	hexvalue(*p++, fpregs[5] + 8, 8);
	hexvalue(*p++, fpregs[5] + 16, 8);
	hexvalue(*p++, fpregs[6], 8);
	hexvalue(*p++, fpregs[6] + 8, 8);
	hexvalue(*p++, fpregs[6] + 16, 8);
	hexvalue(*p++, fpregs[7], 8);
	hexvalue(*p++, fpregs[7] + 8, 8);
	hexvalue(*p++, fpregs[7] + 16, 8);
	hexvalue(ctx->sc_fpcntl[0], fpregs[8], 8);
	hexvalue(ctx->sc_fpcntl[1], fpregs[9], 8);
	hexvalue(ctx->sc_fpcntl[2], fpregs[10], 8);

	/* Generate the output.  */
	ADD_STRING("Register dump:\n\n  D0: ");
	ADD_MEM(regs[0], 8);
	ADD_STRING("  D1: ");
	ADD_MEM(regs[1], 8);
	ADD_STRING("  D2: ");
	ADD_MEM(regs[2], 8);
	ADD_STRING("  D3: ");
	ADD_MEM(regs[3], 8);
	ADD_STRING("\n  D4: ");
	ADD_MEM(regs[4], 8);
	ADD_STRING("  D5: ");
	ADD_MEM(regs[5], 8);
	ADD_STRING("  D6: ");
	ADD_MEM(regs[6], 8);
	ADD_STRING("  D7: ");
	ADD_MEM(regs[7], 8);
	ADD_STRING("\n  A0: ");
	ADD_MEM(regs[8], 8);
	ADD_STRING("  A1: ");
	ADD_MEM(regs[9], 8);
	ADD_STRING("  A2: ");
	ADD_MEM(regs[10], 8);
	ADD_STRING("  A3: ");
	ADD_MEM(regs[11], 8);
	ADD_STRING("\n  A4: ");
	ADD_MEM(regs[12], 8);
	ADD_STRING("  A5: ");
	ADD_MEM(regs[13], 8);
	ADD_STRING("  A6: ");
	ADD_MEM(regs[14], 8);
	ADD_STRING("  A7: ");
	ADD_MEM(regs[15], 8);
	ADD_STRING("\n  PC: ");
	ADD_MEM(regs[16], 8);
	ADD_STRING("  SR: ");
	ADD_MEM(regs[17], 4);

	ADD_STRING("\n\n  OldMask: ");
	ADD_MEM(regs[18], 8);
	ADD_STRING("  Vector: ");
	ADD_MEM(regs[19], 4);

	ADD_STRING("\n\n  FP0: ");
	ADD_MEM(fpregs[0], 24);
	ADD_STRING("  FP1: ");
	ADD_MEM(fpregs[1], 24);
	ADD_STRING("\n  FP2: ");
	ADD_MEM(fpregs[2], 24);
	ADD_STRING("  FP3: ");
	ADD_MEM(fpregs[3], 24);
	ADD_STRING("\n  FP4: ");
	ADD_MEM(fpregs[4], 24);
	ADD_STRING("  FP5: ");
	ADD_MEM(fpregs[5], 24);
	ADD_STRING("\n  FP6: ");
	ADD_MEM(fpregs[6], 24);
	ADD_STRING("  FP7: ");
	ADD_MEM(fpregs[7], 24);
	ADD_STRING("\n  FPCR: ");
	ADD_MEM(fpregs[8], 8);
	ADD_STRING("  FPSR: ");
	ADD_MEM(fpregs[9], 8);
	ADD_STRING("  FPIAR: ");
	ADD_MEM(fpregs[10], 8);
	ADD_STRING("\n");

	/* Write the stuff out.  */
	writev(fd, iov, next_iov - iov);
}

#define REGISTER_DUMP register_dump (fd, ctx)
