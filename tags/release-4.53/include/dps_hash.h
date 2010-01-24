/* Copyright (C) 2003-2010 Datapark corp. All rights reserved.
   Copyright (C) 2000-2002 Lavtech.com corp. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/

#ifndef _DPS_HASH_H
#define _DPS_HASH_H

#include "dps_config.h"

/* Returns unsigned  hash64 of data block */
extern dpshash64_t DpsHash64(const char * buf, size_t size);
extern __C_LINK dpshash32_t __DPSCALL DpsHash32(const char * buf, size_t size);
extern __C_LINK dpshash32_t __DPSCALL DpsHash32Update(const dpshash32_t prev, const char * buf, size_t size);

#define DpsStrHash64(buf)		DpsHash64((buf), dps_strlen(buf))
#define DpsStrHash32(buf)		DpsHash32((buf), dps_strlen(buf))


#ifdef WITH_OLDHASH

extern dpshash32_t DpsOldHash32(const char * buf, size_t size);
#define DpsStrOldHash32(buf)		DpsOldHash32((buf), dps_strlen(buf))

#endif


#endif
