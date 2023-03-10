/* BFD support for the STM8 processor.
   Copyright (C) 2007-2016 Free Software Foundation, Inc.
   Written by Åke Rehnman (at) gmail dot com

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

const bfd_arch_info_type bfd_stm8_arch =
{
  8,                           /* bits per word */
  32,                           /* bits per address */
//  24,                           /* bits per address */
  8,                            /* bits per byte */
  bfd_arch_stm8,                /* architecture */
  bfd_mach_stm8,                /* machine */
  "stm8",                       /* architecture name */
  "stm8",                       /* printable name */
  4,                            /* section align power */
  TRUE,                         /* the default ? */
  bfd_default_compatible,       /* architecture comparison fn */
  bfd_default_scan,             /* string to architecture convert fn */
  bfd_arch_default_fill,
  0,
};
