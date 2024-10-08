/* Copyright (C) 1992,93,94,95,96,97,98,99,2001 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <stdio_ext.h>
#include <termios.h>
#include <unistd.h>

#ifdef USE_IN_LIBIO
# include <wchar.h>
# define flockfile(s) _IO_flockfile (s)
# define funlockfile(s) _IO_funlockfile (s)
#endif

/* It is desirable to use this bit on systems that have it.
   The only bit of terminal state we want to twiddle is echoing, which is
   done in software; there is no need to change the state of the terminal
   hardware.  */

#ifndef TCSASOFT
#define TCSASOFT 0
#endif

char *getpass(prompt)
const char *prompt;
{
	FILE *in, *out;
	struct termios s, t;
	int tty_changed;
	static char *buf;
	static size_t bufsize;
	ssize_t nread;

	/* Try to write to and read from the terminal if we can.
	   If we can't open the terminal, use stderr and stdin.  */

	in = fopen("/dev/tty", "w+");

	if (in == NULL) {
		in = stdin;
		out = stderr;

	} else {
		/* We do the locking ourselves.  */
		__fsetlocking(in, FSETLOCKING_BYCALLER);

		out = in;
	}

	flockfile(out);

	/* Turn echoing off if it is on now.  */

	if (__tcgetattr(fileno(in), &t) == 0) {
		/* Save the old one. */
		s = t;
		/* Tricky, tricky. */
		t.c_lflag &= ~(ECHO | ISIG);
		tty_changed = (tcsetattr(fileno(in), TCSAFLUSH | TCSASOFT, &t) == 0);

	} else
		tty_changed = 0;

	/* Write the prompt.  */
#ifdef USE_IN_LIBIO

	if (_IO_fwide(out, 0) > 0)
		__fwprintf(out, L"%s", prompt);

	else
#endif
		fputs_unlocked(prompt, out);

	fflush_unlocked(out);

	/* Read the password.  */
	nread = __getline(&buf, &bufsize, in);

	if (buf != NULL) {
		if (nread < 0)
			buf[0] = '\0';

		else if (buf[nread - 1] == '\n') {
			/* Remove the newline.  */
			buf[nread - 1] = '\0';

			if (tty_changed) {
				/* Write the newline that was not echoed.  */
#ifdef USE_IN_LIBIO
				if (_IO_fwide(out, 0) > 0)
					putwc_unlocked(L'\n', out);

				else
#endif
					putc_unlocked('\n', out);
			}
		}
	}

	/* Restore the original setting.  */
	if (tty_changed)
		(void) tcsetattr(fileno(in), TCSAFLUSH | TCSASOFT, &s);

	funlockfile(out);

	if (in != stdin)
		/* We opened the terminal; now close it.  */
		fclose(in);

	return buf;
}
