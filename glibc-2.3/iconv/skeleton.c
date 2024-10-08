/* Skeleton for a conversion module.
   Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1998.

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

/* This file can be included to provide definitions of several things
   many modules have in common.  It can be customized using the following
   macros:

     DEFINE_INIT	define the default initializer.  This requires the
			following symbol to be defined.

     CHARSET_NAME	string with official name of the coded character
			set (in all-caps)

     DEFINE_FINI	define the default destructor function.

     MIN_NEEDED_FROM	minimal number of bytes needed for the from-charset.
     MIN_NEEDED_TO	likewise for the to-charset.

     MAX_NEEDED_FROM	maximal number of bytes needed for the from-charset.
			This macro is optional, it defaults to MIN_NEEDED_FROM.
     MAX_NEEDED_TO	likewise for the to-charset.

     FROM_LOOP_MIN_NEEDED_FROM
     FROM_LOOP_MAX_NEEDED_FROM
                        minimal/maximal number of bytes needed on input
                        of one round through the FROM_LOOP.  Defaults
                        to MIN_NEEDED_FROM and MAX_NEEDED_FROM, respectively.
     FROM_LOOP_MIN_NEEDED_TO
     FROM_LOOP_MAX_NEEDED_TO
                        minimal/maximal number of bytes needed on output
                        of one round through the FROM_LOOP.  Defaults
                        to MIN_NEEDED_TO and MAX_NEEDED_TO, respectively.
     TO_LOOP_MIN_NEEDED_FROM
     TO_LOOP_MAX_NEEDED_FROM
                        minimal/maximal number of bytes needed on input
                        of one round through the TO_LOOP.  Defaults
                        to MIN_NEEDED_TO and MAX_NEEDED_TO, respectively.
     TO_LOOP_MIN_NEEDED_TO
     TO_LOOP_MAX_NEEDED_TO
                        minimal/maximal number of bytes needed on output
                        of one round through the TO_LOOP.  Defaults
                        to MIN_NEEDED_FROM and MAX_NEEDED_FROM, respectively.

     DEFINE_DIRECTION_OBJECTS
			two objects will be defined to be used when the
			`gconv' function must only distinguish two
			directions.  This is implied by DEFINE_INIT.
			If this macro is not defined the following
			macro must be available.

     FROM_DIRECTION	this macro is supposed to return a value != 0
			if we convert from the current character set,
			otherwise it return 0.

     EMIT_SHIFT_TO_INIT	this symbol is optional.  If it is defined it
			defines some code which writes out a sequence
			of bytes which bring the current state into
			the initial state.

     FROM_LOOP		name of the function implementing the conversion
			from the current character set.
     TO_LOOP		likewise for the other direction

     ONE_DIRECTION	optional.  If defined to 1, only one conversion
			direction is defined instead of two.  In this
			case, FROM_DIRECTION should be defined to 1, and
			FROM_LOOP and TO_LOOP should have the same value.

     SAVE_RESET_STATE	in case of an error we must reset the state for
			the rerun so this macro must be defined for
			stateful encodings.  It takes an argument which
			is nonzero when saving.

     RESET_INPUT_BUFFER	If the input character sets allow this the macro
			can be defined to reset the input buffer pointers
			to cover only those characters up to the error.

     FUNCTION_NAME	if not set the conversion function is named `gconv'.

     PREPARE_LOOP	optional code preparing the conversion loop.  Can
			contain variable definitions.
     END_LOOP		also optional, may be used to store information

     EXTRA_LOOP_ARGS	optional macro specifying extra arguments passed
			to loop function.

   Modules can use mbstate_t to store conversion state as follows:

   * Bits 2..0 of '__count' contain the number of lookahead input bytes
     stored in __value.__wchb.  Always zero if the converter never
     returns __GCONV_INCOMPLETE_INPUT.

   * Bits 31..3 of '__count' are module dependent shift state.

   * __value: When STORE_REST/UNPACK_BYTES aren't defined and when the
     converter has returned __GCONV_INCOMPLETE_INPUT, this contains
     at most 4 lookahead bytes. Converters with an mb_cur_max > 4
     (currently only UTF-8) must find a way to store their state
     in __value.__wch and define STORE_REST/UNPACK_BYTES appropriately.

   When __value contains lookahead, __count must not be zero, because
   the converter is not in the initial state then, and mbsinit() --
   defined as a (__count == 0) test -- must reflect this.
 */

#include <assert.h>
#include <gconv.h>
#include <string.h>
#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifndef STATIC_GCONV
# include <dlfcn.h>
#endif

#ifndef DL_CALL_FCT
# define DL_CALL_FCT(fct, args) fct args
#endif

/* The direction objects.  */
#if DEFINE_DIRECTION_OBJECTS || DEFINE_INIT
static int from_object;
static int to_object;

# ifndef FROM_DIRECTION
#  define FROM_DIRECTION (step->__data == &from_object)
# endif
#else
# ifndef FROM_DIRECTION
#  error "FROM_DIRECTION must be provided if direction objects are not used"
# endif
#endif


/* How many bytes are needed at most for the from-charset.  */
#ifndef MAX_NEEDED_FROM
# define MAX_NEEDED_FROM	MIN_NEEDED_FROM
#endif

/* Same for the to-charset.  */
#ifndef MAX_NEEDED_TO
# define MAX_NEEDED_TO		MIN_NEEDED_TO
#endif

/* Defaults for the per-direction min/max constants.  */
#ifndef FROM_LOOP_MIN_NEEDED_FROM
# define FROM_LOOP_MIN_NEEDED_FROM	MIN_NEEDED_FROM
#endif
#ifndef FROM_LOOP_MAX_NEEDED_FROM
# define FROM_LOOP_MAX_NEEDED_FROM	MAX_NEEDED_FROM
#endif
#ifndef FROM_LOOP_MIN_NEEDED_TO
# define FROM_LOOP_MIN_NEEDED_TO	MIN_NEEDED_TO
#endif
#ifndef FROM_LOOP_MAX_NEEDED_TO
# define FROM_LOOP_MAX_NEEDED_TO	MAX_NEEDED_TO
#endif
#ifndef TO_LOOP_MIN_NEEDED_FROM
# define TO_LOOP_MIN_NEEDED_FROM	MIN_NEEDED_TO
#endif
#ifndef TO_LOOP_MAX_NEEDED_FROM
# define TO_LOOP_MAX_NEEDED_FROM	MAX_NEEDED_TO
#endif
#ifndef TO_LOOP_MIN_NEEDED_TO
# define TO_LOOP_MIN_NEEDED_TO		MIN_NEEDED_FROM
#endif
#ifndef TO_LOOP_MAX_NEEDED_TO
# define TO_LOOP_MAX_NEEDED_TO		MAX_NEEDED_FROM
#endif


/* Define macros which can access unaligned buffers.  These macros are
   supposed to be used only in code outside the inner loops.  For the inner
   loops we have other definitions which allow optimized access.  */
#ifdef _STRING_ARCH_unaligned
/* We can handle unaligned memory access.  */
# define get16u(addr) *((__const uint16_t *) (addr))
# define get32u(addr) *((__const uint32_t *) (addr))

/* We need no special support for writing values either.  */
# define put16u(addr, val) *((uint16_t *) (addr)) = (val)
# define put32u(addr, val) *((uint32_t *) (addr)) = (val)
#else
/* Distinguish between big endian and little endian.  */
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define get16u(addr) \
     (((__const unsigned char *) (addr))[1] << 8			      \
      | ((__const unsigned char *) (addr))[0])
#  define get32u(addr) \
     (((((__const unsigned char *) (addr))[3] << 8			      \
	| ((__const unsigned char *) (addr))[2]) << 8			      \
       | ((__const unsigned char *) (addr))[1]) << 8			      \
      | ((__const unsigned char *) (addr))[0])

#  define put16u(addr, val) \
     ({ uint16_t __val = (val);						      \
	((unsigned char *) (addr))[0] = __val;				      \
	((unsigned char *) (addr))[1] = __val >> 8;			      \
	(void) 0; })
#  define put32u(addr, val) \
     ({ uint32_t __val = (val);						      \
	((unsigned char *) (addr))[0] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[1] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[2] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[3] = __val;				      \
	(void) 0; })
# else
#  define get16u(addr) \
     (((__const unsigned char *) (addr))[0] << 8			      \
      | ((__const unsigned char *) (addr))[1])
#  define get32u(addr) \
     (((((__const unsigned char *) (addr))[0] << 8			      \
	| ((__const unsigned char *) (addr))[1]) << 8			      \
       | ((__const unsigned char *) (addr))[2]) << 8			      \
      | ((__const unsigned char *) (addr))[3])

#  define put16u(addr, val) \
     ({ uint16_t __val = (val);						      \
	((unsigned char *) (addr))[1] = __val;				      \
	((unsigned char *) (addr))[0] = __val >> 8;			      \
	(void) 0; })
#  define put32u(addr, val) \
     ({ uint32_t __val = (val);						      \
	((unsigned char *) (addr))[3] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[2] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[1] = __val;				      \
	__val >>= 8;							      \
	((unsigned char *) (addr))[0] = __val;				      \
	(void) 0; })
# endif
#endif


/* For conversions from a fixed width character set to another fixed width
   character set we can define RESET_INPUT_BUFFER in a very fast way.  */
#if !defined RESET_INPUT_BUFFER && !defined SAVE_RESET_STATE
# if FROM_LOOP_MIN_NEEDED_FROM == FROM_LOOP_MAX_NEEDED_FROM \
     && FROM_LOOP_MIN_NEEDED_TO == FROM_LOOP_MAX_NEEDED_TO \
     && TO_LOOP_MIN_NEEDED_FROM == TO_LOOP_MAX_NEEDED_FROM \
     && TO_LOOP_MIN_NEEDED_TO == TO_LOOP_MAX_NEEDED_TO
/* We have to use these `if's here since the compiler cannot know that
   (outbuf - outerr) is always divisible by FROM/TO_LOOP_MIN_NEEDED_TO.
   The ?:1 avoids division by zero warnings that gcc 3.2 emits even for
   obviously unreachable code.  */
#  define RESET_INPUT_BUFFER \
  if (FROM_DIRECTION)							      \
    {									      \
      if (FROM_LOOP_MIN_NEEDED_FROM % FROM_LOOP_MIN_NEEDED_TO == 0)	      \
	*inptrp -= (outbuf - outerr)					      \
		   * (FROM_LOOP_MIN_NEEDED_FROM / FROM_LOOP_MIN_NEEDED_TO);   \
      else if (FROM_LOOP_MIN_NEEDED_TO % FROM_LOOP_MIN_NEEDED_FROM == 0)      \
	*inptrp -= (outbuf - outerr)					      \
		   / (FROM_LOOP_MIN_NEEDED_TO / FROM_LOOP_MIN_NEEDED_FROM     \
		      ? : 1);						      \
      else								      \
	*inptrp -= ((outbuf - outerr) / FROM_LOOP_MIN_NEEDED_TO)	      \
		   * FROM_LOOP_MIN_NEEDED_FROM;				      \
    }									      \
  else									      \
    {									      \
      if (TO_LOOP_MIN_NEEDED_FROM % TO_LOOP_MIN_NEEDED_TO == 0)		      \
	*inptrp -= (outbuf - outerr)					      \
		   * (TO_LOOP_MIN_NEEDED_FROM / TO_LOOP_MIN_NEEDED_TO);	      \
      else if (TO_LOOP_MIN_NEEDED_TO % TO_LOOP_MIN_NEEDED_FROM == 0)	      \
	*inptrp -= (outbuf - outerr)					      \
		   / (TO_LOOP_MIN_NEEDED_TO / TO_LOOP_MIN_NEEDED_FROM ? : 1); \
      else								      \
	*inptrp -= ((outbuf - outerr) / TO_LOOP_MIN_NEEDED_TO)		      \
		   * TO_LOOP_MIN_NEEDED_FROM;				      \
    }
# endif
#endif


/* The default init function.  It simply matches the name and initializes
   the step data to point to one of the objects above.  */
#if DEFINE_INIT
# ifndef CHARSET_NAME
#  error "CHARSET_NAME not defined"
# endif

extern int gconv_init(struct __gconv_step *step);
int gconv_init(struct __gconv_step *step)
{
	/* Determine which direction.  */
	if (strcmp(step->__from_name, CHARSET_NAME) == 0) {
		step->__data = &from_object;

		step->__min_needed_from = FROM_LOOP_MIN_NEEDED_FROM;
		step->__max_needed_from = FROM_LOOP_MAX_NEEDED_FROM;
		step->__min_needed_to = FROM_LOOP_MIN_NEEDED_TO;
		step->__max_needed_to = FROM_LOOP_MAX_NEEDED_TO;

	} else if (__builtin_expect(strcmp(step->__to_name, CHARSET_NAME), 0) == 0) {
		step->__data = &to_object;

		step->__min_needed_from = TO_LOOP_MIN_NEEDED_FROM;
		step->__max_needed_from = TO_LOOP_MAX_NEEDED_FROM;
		step->__min_needed_to = TO_LOOP_MIN_NEEDED_TO;
		step->__max_needed_to = TO_LOOP_MAX_NEEDED_TO;

	} else
		return __GCONV_NOCONV;

#ifdef SAVE_RESET_STATE
	step->__stateful = 1;
#else
	step->__stateful = 0;
#endif

	return __GCONV_OK;
}

#endif


/* The default destructor function does nothing in the moment and so
   we don't define it at all.  But we still provide the macro just in
   case we need it some day.  */
#if DEFINE_FINI
#endif


/* If no arguments have to passed to the loop function define the macro
   as empty.  */
#ifndef EXTRA_LOOP_ARGS
# define EXTRA_LOOP_ARGS
#endif


/* This is the actual conversion function.  */
#ifndef FUNCTION_NAME
# define FUNCTION_NAME	gconv
#endif

/* The macros are used to access the function to convert single characters.  */
#define SINGLE(fct) SINGLE2 (fct)
#define SINGLE2(fct) fct##_single


extern int FUNCTION_NAME(struct __gconv_step *step,
                         struct __gconv_step_data *data,
                         const unsigned char **inptrp,
                         const unsigned char *inend,
                         unsigned char **outbufstart, size_t *irreversible,
                         int do_flush, int consume_incomplete);
int FUNCTION_NAME(struct __gconv_step *step, struct __gconv_step_data *data,
                  const unsigned char **inptrp, const unsigned char *inend,
                  unsigned char **outbufstart, size_t *irreversible, int do_flush,
                  int consume_incomplete)
{
	struct __gconv_step *next_step = step + 1;
	struct __gconv_step_data *next_data = data + 1;
	__gconv_fct fct;
	int status;

	fct = (data->__flags & __GCONV_IS_LAST) ? NULL : next_step->__fct;

	/* If the function is called with no input this means we have to reset
	   to the initial state.  The possibly partly converted input is
	   dropped.  */
	if (__builtin_expect(do_flush, 0)) {
		/* This should never happen during error handling.  */
		assert(outbufstart == NULL);

		status = __GCONV_OK;

#ifdef EMIT_SHIFT_TO_INIT

		if (do_flush == 1) {
			/* We preserve the initial values of the pointer variables.  */
			unsigned char *outbuf = data->__outbuf;
			unsigned char *outstart = outbuf;
			unsigned char *outend = data->__outbufend;

# ifdef PREPARE_LOOP
			PREPARE_LOOP
# endif

# ifdef SAVE_RESET_STATE
			SAVE_RESET_STATE(1);
# endif

			/* Emit the escape sequence to reset the state.  */
			EMIT_SHIFT_TO_INIT;

			/* Call the steps down the chain if there are any but only if we
			   successfully emitted the escape sequence.  This should only
			   fail if the output buffer is full.  If the input is invalid
			   it should be discarded since the user wants to start from a
			   clean state.  */
			if (status == __GCONV_OK) {
				if (data->__flags & __GCONV_IS_LAST)
					/* Store information about how many bytes are available.  */
					data->__outbuf = outbuf;

				else {
					/* Write out all output which was produced.  */
					if (outbuf > outstart) {
						const unsigned char *outerr = outstart;
						int result;

						result = DL_CALL_FCT(fct, (next_step, next_data,
						                           &outerr, outbuf, NULL,
						                           irreversible, 0,
						                           consume_incomplete));

						if (result != __GCONV_EMPTY_INPUT) {
							if (__builtin_expect(outerr != outbuf, 0)) {
								/* We have a problem.  Undo the conversion.  */
								outbuf = outstart;

								/* Restore the state.  */
# ifdef SAVE_RESET_STATE
								SAVE_RESET_STATE(0);
# endif
							}

							/* Change the status.  */
							status = result;
						}
					}

					if (status == __GCONV_OK)
						/* Now flush the remaining steps.  */
						status = DL_CALL_FCT(fct, (next_step, next_data, NULL,
						                           NULL, NULL, irreversible, 1,
						                           consume_incomplete));
				}
			}

		} else
#endif
		{
			/* Clear the state object.  There might be bytes in there from
			   previous calls with CONSUME_INCOMPLETE == 1.  But don't emit
			   escape sequences.  */
			memset(data->__statep, '\0', sizeof(*data->__statep));

			if (!(data->__flags & __GCONV_IS_LAST))
				/* Now flush the remaining steps.  */
				status = DL_CALL_FCT(fct, (next_step, next_data, NULL, NULL,
				                           NULL, irreversible, do_flush,
				                           consume_incomplete));
		}

	} else {
		/* We preserve the initial values of the pointer variables.  */
		const unsigned char *inptr = *inptrp;
		unsigned char *outbuf = (__builtin_expect(outbufstart == NULL, 1)
		                         ? data->__outbuf : *outbufstart);
		unsigned char *outend = data->__outbufend;
		unsigned char *outstart;
		/* This variable is used to count the number of characters we
		 actually converted.  */
		size_t lirreversible = 0;
		size_t *lirreversiblep = irreversible ? &lirreversible : NULL;

		/* The following assumes that encodings, which have a variable length
		 what might unalign a buffer even though it is a aligned in the
		 beginning, either don't have the minimal number of bytes as a divisor
		 of the maximum length or have a minimum length of 1.  This is true
		 for all known and supported encodings.
		 We use && instead of || to combine the subexpression for the FROM
		 encoding and for the TO encoding, because usually one of them is
		 INTERNAL, for which the subexpression evaluates to 1, but INTERNAL
		 buffers are always aligned correctly.  */
#define POSSIBLY_UNALIGNED \
  (!defined _STRING_ARCH_unaligned					      \
   && (((FROM_LOOP_MIN_NEEDED_FROM != 1					      \
	 && FROM_LOOP_MAX_NEEDED_FROM % FROM_LOOP_MIN_NEEDED_FROM == 0)	      \
	&& (FROM_LOOP_MIN_NEEDED_TO != 1				      \
	    && FROM_LOOP_MAX_NEEDED_TO % FROM_LOOP_MIN_NEEDED_TO == 0))	      \
       || ((TO_LOOP_MIN_NEEDED_FROM != 1				      \
	    && TO_LOOP_MAX_NEEDED_FROM % TO_LOOP_MIN_NEEDED_FROM == 0)	      \
	   && (TO_LOOP_MIN_NEEDED_TO != 1				      \
	       && TO_LOOP_MAX_NEEDED_TO % TO_LOOP_MIN_NEEDED_TO == 0))))
#if POSSIBLY_UNALIGNED
		int unaligned;
# define GEN_unaligned(name) GEN_unaligned2 (name)
# define GEN_unaligned2(name) name##_unaligned
#else
# define unaligned 0
#endif

#ifdef PREPARE_LOOP
		PREPARE_LOOP
#endif

#if FROM_LOOP_MAX_NEEDED_FROM > 1 || TO_LOOP_MAX_NEEDED_FROM > 1

		/* If the function is used to implement the mb*towc*() or wc*tomb*()
		 functions we must test whether any bytes from the last call are
		 stored in the `state' object.  */
		if (((FROM_LOOP_MAX_NEEDED_FROM > 1 && TO_LOOP_MAX_NEEDED_FROM > 1)
		                || (FROM_LOOP_MAX_NEEDED_FROM > 1 && FROM_DIRECTION)
		                || (TO_LOOP_MAX_NEEDED_FROM > 1 && !FROM_DIRECTION))
		                && consume_incomplete && (data->__statep->__count & 7) != 0) {
			/* Yep, we have some bytes left over.  Process them now.
			   But this must not happen while we are called from an
			   error handler.  */
			assert(outbufstart == NULL);

# if FROM_LOOP_MAX_NEEDED_FROM > 1

			if (TO_LOOP_MAX_NEEDED_FROM == 1 || FROM_DIRECTION)
				status = SINGLE(FROM_LOOP)(step, data, inptrp, inend, &outbuf,
				                           outend, lirreversiblep
				                           EXTRA_LOOP_ARGS);

# endif
# if !ONE_DIRECTION
#  if FROM_LOOP_MAX_NEEDED_FROM > 1 && TO_LOOP_MAX_NEEDED_FROM > 1
			else
#  endif
#  if TO_LOOP_MAX_NEEDED_FROM > 1
				status = SINGLE(TO_LOOP)(step, data, inptrp, inend, &outbuf,
				                         outend, lirreversiblep EXTRA_LOOP_ARGS);

#  endif
# endif

			if (__builtin_expect(status, __GCONV_OK) != __GCONV_OK)
				return status;
		}

#endif

#if POSSIBLY_UNALIGNED
		unaligned =
		        ((FROM_DIRECTION
		          && ((uintptr_t) inptr % FROM_LOOP_MIN_NEEDED_FROM != 0
		              || ((data->__flags & __GCONV_IS_LAST)
		                  && (uintptr_t) outbuf % FROM_LOOP_MIN_NEEDED_TO != 0)))
		         || (!FROM_DIRECTION
		             && (((data->__flags & __GCONV_IS_LAST)
		                  && (uintptr_t) outbuf % TO_LOOP_MIN_NEEDED_TO != 0)
		                 || (uintptr_t) inptr % TO_LOOP_MIN_NEEDED_FROM != 0)));
#endif

		while (1) {
			struct __gconv_trans_data *trans;

			/* Remember the start value for this round.  */
			inptr = *inptrp;
			/* The outbuf buffer is empty.  */
			outstart = outbuf;

#ifdef SAVE_RESET_STATE
			SAVE_RESET_STATE(1);
#endif

			if (__builtin_expect(!unaligned, 1)) {
				if (FROM_DIRECTION)
					/* Run the conversion loop.  */
					status = FROM_LOOP(step, data, inptrp, inend, &outbuf, outend,
					                   lirreversiblep EXTRA_LOOP_ARGS);

				else
					/* Run the conversion loop.  */
					status = TO_LOOP(step, data, inptrp, inend, &outbuf, outend,
					                 lirreversiblep EXTRA_LOOP_ARGS);
			}

#if POSSIBLY_UNALIGNED

			else {
				if (FROM_DIRECTION)
					/* Run the conversion loop.  */
					status = GEN_unaligned(FROM_LOOP)(step, data, inptrp, inend,
					                                  &outbuf, outend,
					                                  lirreversiblep
					                                  EXTRA_LOOP_ARGS);

				else
					/* Run the conversion loop.  */
					status = GEN_unaligned(TO_LOOP)(step, data, inptrp, inend,
					                                &outbuf, outend,
					                                lirreversiblep
					                                EXTRA_LOOP_ARGS);
			}

#endif

			/* If we were called as part of an error handling module we
			   don't do anything else here.  */
			if (__builtin_expect(outbufstart != NULL, 0)) {
				*outbufstart = outbuf;
				return status;
			}

			/* Give the transliteration module the chance to store the
			   original text and the result in case it needs a context.  */
			for (trans = data->__trans; trans != NULL; trans = trans->__next)
				if (trans->__trans_context_fct != NULL)
					DL_CALL_FCT(trans->__trans_context_fct,
					            (trans->__data, inptr, *inptrp, outstart, outbuf));

			/* We finished one use of the loops.  */
			++data->__invocation_counter;

			/* If this is the last step leave the loop, there is nothing
			   we can do.  */
			if (__builtin_expect(data->__flags & __GCONV_IS_LAST, 0)) {
				/* Store information about how many bytes are available.  */
				data->__outbuf = outbuf;

				/* Remember how many non-identical characters we
				   converted in a irreversible way.  */
				*irreversible += lirreversible;

				break;
			}

			/* Write out all output which was produced.  */
			if (__builtin_expect(outbuf > outstart, 1)) {
				const unsigned char *outerr = data->__outbuf;
				int result;

				result = DL_CALL_FCT(fct, (next_step, next_data, &outerr,
				                           outbuf, NULL, irreversible, 0,
				                           consume_incomplete));

				if (result != __GCONV_EMPTY_INPUT) {
					if (__builtin_expect(outerr != outbuf, 0)) {
#ifdef RESET_INPUT_BUFFER
						RESET_INPUT_BUFFER;
#else
						/* We have a problem with the in on of the functions
						 below.  Undo the conversion upto the error point.  */
						size_t nstatus;

						/* Reload the pointers.  */
						*inptrp = inptr;
						outbuf = outstart;

						/* Restore the state.  */
# ifdef SAVE_RESET_STATE
						SAVE_RESET_STATE(0);
# endif

						if (__builtin_expect(!unaligned, 1)) {
							if (FROM_DIRECTION)
								/* Run the conversion loop.  */
								nstatus = FROM_LOOP(step, data, inptrp, inend,
								                    &outbuf, outerr,
								                    lirreversiblep
								                    EXTRA_LOOP_ARGS);

							else
								/* Run the conversion loop.  */
								nstatus = TO_LOOP(step, data, inptrp, inend,
								                  &outbuf, outerr,
								                  lirreversiblep
								                  EXTRA_LOOP_ARGS);
						}

# if POSSIBLY_UNALIGNED

						else {
							if (FROM_DIRECTION)
								/* Run the conversion loop.  */
								nstatus = GEN_unaligned(FROM_LOOP)(step, data,
								                                   inptrp, inend,
								                                   &outbuf,
								                                   outerr,
								                                   lirreversiblep
								                                   EXTRA_LOOP_ARGS);

							else
								/* Run the conversion loop.  */
								nstatus = GEN_unaligned(TO_LOOP)(step, data,
								                                 inptrp, inend,
								                                 &outbuf, outerr,
								                                 lirreversiblep
								                                 EXTRA_LOOP_ARGS);
						}

# endif

						/* We must run out of output buffer space in this
						 rerun.  */
						assert(outbuf == outerr);
						assert(nstatus == __GCONV_FULL_OUTPUT);

						/* If we haven't consumed a single byte decrement
						 the invocation counter.  */
						if (__builtin_expect(outbuf == outstart, 0))
							--data->__invocation_counter;

#endif	/* reset input buffer */
					}

					/* Change the status.  */
					status = result;

				} else

					/* All the output is consumed, we can make another run
					   if everything was ok.  */
					if (status == __GCONV_FULL_OUTPUT) {
						status = __GCONV_OK;
						outbuf = data->__outbuf;
					}
			}

			if (status != __GCONV_OK)
				break;

			/* Reset the output buffer pointer for the next round.  */
			outbuf = data->__outbuf;
		}

#ifdef END_LOOP
		END_LOOP
#endif

		/* If we are supposed to consume all character store now all of the
		 remaining characters in the `state' object.  */
#if FROM_LOOP_MAX_NEEDED_FROM > 1 || TO_LOOP_MAX_NEEDED_FROM > 1

		if (((FROM_LOOP_MAX_NEEDED_FROM > 1 && TO_LOOP_MAX_NEEDED_FROM > 1)
		                || (FROM_LOOP_MAX_NEEDED_FROM > 1 && FROM_DIRECTION)
		                || (TO_LOOP_MAX_NEEDED_FROM > 1 && !FROM_DIRECTION))
		                && __builtin_expect(consume_incomplete, 0)
		                && status == __GCONV_INCOMPLETE_INPUT) {
# ifdef STORE_REST
			mbstate_t *state = data->__statep;

			STORE_REST
# else
			size_t cnt;

			/* Make sure the remaining bytes fit into the state objects
			   buffer.  */
			assert(inend - *inptrp < 4);

			for (cnt = 0; *inptrp < inend; ++cnt)
				data->__statep->__value.__wchb[cnt] = *(*inptrp)++;

			data->__statep->__count &= ~7;
			data->__statep->__count |= cnt;
# endif
		}

#endif
#undef unaligned
#undef POSSIBLY_UNALIGNED
	}

	return status;
}

#undef DEFINE_INIT
#undef CHARSET_NAME
#undef DEFINE_FINI
#undef MIN_NEEDED_FROM
#undef MIN_NEEDED_TO
#undef MAX_NEEDED_FROM
#undef MAX_NEEDED_TO
#undef FROM_LOOP_MIN_NEEDED_FROM
#undef FROM_LOOP_MAX_NEEDED_FROM
#undef FROM_LOOP_MIN_NEEDED_TO
#undef FROM_LOOP_MAX_NEEDED_TO
#undef TO_LOOP_MIN_NEEDED_FROM
#undef TO_LOOP_MAX_NEEDED_FROM
#undef TO_LOOP_MIN_NEEDED_TO
#undef TO_LOOP_MAX_NEEDED_TO
#undef DEFINE_DIRECTION_OBJECTS
#undef FROM_DIRECTION
#undef EMIT_SHIFT_TO_INIT
#undef FROM_LOOP
#undef TO_LOOP
#undef SAVE_RESET_STATE
#undef RESET_INPUT_BUFFER
#undef FUNCTION_NAME
#undef PREPARE_LOOP
#undef END_LOOP
#undef ONE_DIRECTION
#undef STORE_REST
