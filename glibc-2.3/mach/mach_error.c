/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: mach_error.c,v $
 * Revision 1.2  1997/03/16 17:42:02  drepper
 * (mach_error_string_int): Give full prototype.
 *
 * Revision 1.2  1997/03/14 15:26:30  thomas
 * Wed Mar  5 10:40:05 1997  Thomas Bushnell, n/BSG  <thomas@gnu.ai.mit.edu>
 *
 * 	* mach/mach_error.c (mach_error_string_int): Give full prototype.
 * 	* mach/errstring.c (mach_error_string_int): Likewise.
 * 	* mach/error_compat.c (__mach_error_map_compat): Likewise.
 *
 * 	* mach/spin-solid.c: Include <mach/mach_traps.h>.
 * 	* mach/spin-solid.c (__spin_lock_solid): Provide arg to
 * 	swtch_pri.
 *
 * 	* mach/mach_init.c: Include <mach/mig_support.h>.
 *
 * 	* mach/mach_error.h (mach_error_string, mach_error,
 * 	mach_error_type): Always provide prototypes.
 *
 * Revision 1.1  1992/10/06 18:29:54  roland
 * entered into RCS
 *
 * Revision 2.4  92/02/19  15:10:52  elf
 * 	Moved mach_error_string and mach_error_type to mach_error_string.c.
 * 	[92/02/11            rpd]
 *
 * Revision 2.3  92/01/23  15:22:06  rpd
 * 	Changed <servers/errorlib.h> to <errorlib.h>.
 * 	[92/01/16            rpd]
 *
 * Revision 2.2  92/01/16  00:08:03  rpd
 * 	Moved from user collection to mk collection.
 *
 * Revision 2.3  91/08/29  15:51:50  rpd
 * 	Changed IPC_MIG_MOD to MACH_IPC_MIG_MOD, to get the new error strings.
 * 	[91/08/22            rpd]
 *
 * Revision 2.2  91/03/27  16:06:29  mrt
 * 	Changed include of "errorlib.h" to <servers/errorlib.h>
 * 	Added new copyright
 * 	[91/03/20            mrt]
 *
 */
/*
 * 	File:	mach_error.c
 *	Author:	Douglas Orr, Carnegie Mellon University
 *	Date:	Mar 1988
 *
 *      interprets structured mach error codes and prints
 *      or returns a descriptive string.
 */

#include <stdio.h>
#include <mach_error.h>
#include <mach/boolean.h>

extern char *mach_error_string_int(mach_error_t, boolean_t *);

void mach_error(str, err)
char	*str;
mach_error_t		err;
{
	char *err_str;
	char buf[1024];
	boolean_t diag;

	err_str = mach_error_string_int(err, &diag);

	if (diag) {
		sprintf(buf, "%s %s (%x)", mach_error_type(err), err_str, err);
		err_str = buf;
	}

	fprintf(stderr, "%s %s\n", str, err_str);
}
