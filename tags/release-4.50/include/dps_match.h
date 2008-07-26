/* Copyright (C) 2003, Datapark corp. All rights reserved.
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

#ifndef _DPS_MATCH_H
#define _DPS_MATCH_H

extern int  DpsMatchComp(DPS_MATCH *Match, char *errstr, size_t errstrsize);
extern void DpsMatchFree(DPS_MATCH *Match);
extern int  DpsMatchExec(DPS_MATCH *Match, const char *string, const char *net_string, 
			 struct sockaddr_in *sin, size_t nparts, DPS_MATCH_PART *Parts);
extern int  DpsMatchApply(char *res,size_t ressize,const char *src,const char *rpl,DPS_MATCH *Match,size_t nparts, DPS_MATCH_PART *Parts);

extern DPS_MATCHLIST *DpsMatchListInit(DPS_MATCHLIST *L);
extern void DpsMatchListFree(DPS_MATCHLIST *L);
extern int  DpsMatchListAdd(DPS_AGENT *A, DPS_MATCHLIST *L, DPS_MATCH *M, char *err, size_t errsize, int ordre);

extern __C_LINK DPS_MATCH * __DPSCALL DpsMatchListFind(DPS_MATCHLIST * List,const char *string,size_t nparts, DPS_MATCH_PART *Parts);
extern DPS_MATCH * __DPSCALL DpsSectionMatchListFind(DPS_MATCHLIST *L, DPS_DOCUMENT *Doc, size_t nparts, DPS_MATCH_PART *Parts);

extern const char *DpsMatchTypeStr(int m);

extern DPS_MATCH *DpsMatchInit(DPS_MATCH *M);

#endif
