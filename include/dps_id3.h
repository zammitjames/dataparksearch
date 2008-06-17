/* Copyright (C) 2003 Datapark corp. All rights reserved.
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

#ifndef _DPS_ID3_H
#define _DPS_ID3_H

#define DPS_MP3_UNKNOWN	0
#define DPS_MP3_TAG	1
#define DPS_MP3_ID3	2
#define DPS_MP3_RIFF	3

int DpsMP3Type(DPS_DOCUMENT *Doc);
int DpsMP3Parse(DPS_AGENT *A, DPS_DOCUMENT *D);

#endif
