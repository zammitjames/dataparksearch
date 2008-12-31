/* Copyright (C) 2003-2008 Datapark corp. All rights reserved.
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

#ifndef _DPS_INDEXER_H
#define _DPS_INDEXER_H

/* API fuctions to be called in main() */

extern __C_LINK int  __DPSCALL DpsIndexNextURL(DPS_AGENT *Indexer);
extern __C_LINK int  __DPSCALL DpsIndexSubDoc(DPS_AGENT *Indexer, DPS_DOCUMENT *Parent, const char *base, const char *lang, const char *url);
extern __C_LINK int  __DPSCALL DpsURLFile(DPS_AGENT *Indexer, const char *f, int cmd);
extern int DpsConvertHref(DPS_AGENT *Indexer, DPS_URL *CurURL, DPS_HREF *Href);
extern __C_LINK int __DPSCALL DpsStoreHrefs(DPS_AGENT *Indexer);
extern int DpsDocStoreHrefs(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc);
extern int DpsVarList2Doc(DPS_DOCUMENT *Doc, DPS_SERVER *Server);
extern int DpsHrefCheck(DPS_AGENT *Indexer, DPS_HREF *Href, const char *newhref);

#endif
