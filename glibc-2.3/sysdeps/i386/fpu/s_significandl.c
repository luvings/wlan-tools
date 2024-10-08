/*
 * Written by J.T. Conklin <jtc@netbsd.org>.
 * Changes for long double by Ulrich Drepper <drepper@cygnus.com>
 * Public domain.
 */

#include <math_private.h>

long double __significandl(long double x)
{
	long double res;

	asm("fxtract\n"
	    "fstp	%%st(0)" : "=t"(res) : "0"(x));
	return res;
}

weak_alias(__significandl, significandl)
