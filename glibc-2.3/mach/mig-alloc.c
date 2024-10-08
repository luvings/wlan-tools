/* Copyright (C) 1992, 1995, 1997 Free Software Foundation, Inc.
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

#include <mach.h>

/* Called by MiG to allocate space.  */
void __mig_allocate(vm_address_t *addr,
                    vm_size_t size)
{
	if (__vm_allocate(__mach_task_self(), addr, size, 1) != KERN_SUCCESS)
		*addr = 0;
}

weak_alias(__mig_allocate, mig_allocate)
