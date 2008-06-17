/* Copyright (C) 2003-2006 Datapark corp. All rights reserved.
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

#include "dps_common.h"
#include "dps_contentencoding.h"
#include "dps_utils.h"
#include "dps_xmalloc.h"
#include "dps_charsetutils.h"
#include "dps_log.h"
#include "dps_vars.h"
#include "dps_proto.h"

#ifdef HAVE_ZLIB

#include <stdlib.h>
#include <string.h>
#include <zlib.h>

__C_LINK int __DPSCALL DpsInflate(DPS_AGENT *query, DPS_DOCUMENT *Doc) {

  z_stream zstream;
  size_t csize, gap;
  Byte *pfree;
  int rc;
  
  if( (Doc->Buf.size) <= (size_t)(Doc->Buf.content - Doc->Buf.buf + 6) )
    return -1;

  gap = (Doc->Buf.content - Doc->Buf.buf)/* + 1*/;
  csize = Doc->Buf.size - gap;

  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;

  inflateInit2(&zstream, -MAX_WBITS);

  if ((pfree = zstream.next_out = (Byte*)DpsMalloc((size_t)Doc->Buf.allocated_size + 1)) == NULL) {
    inflateEnd(&zstream);
    return -1;
  }
  if ((Doc->Buf.content[0] == (char)0x1f) && (Doc->Buf.content[1] == (char)0x8b)) {
    zstream.next_in = (Byte*)&Doc->Buf.content[2];
    /* 2 bytes - header, 4 bytes - trailing CRC */
    zstream.avail_in = csize - 6;   
  } else {
    zstream.next_in = (Byte*)Doc->Buf.content;
    zstream.avail_in = csize;
  }

  dps_memmove(zstream.next_out, Doc->Buf.buf, gap);

  zstream.next_out += gap;

  zstream.avail_out = (uLong)(Doc->Buf.allocated_size - gap);

  while(1) {
    rc = inflate(&zstream, Z_NO_FLUSH);
    if (rc == Z_OK) {
      if (Doc->Buf.allocated_size > Doc->Buf.max_size) {
	DpsLog(query, DPS_LOG_EXTRA, "Inflate: too large content");
	DpsVarListReplaceInt(&Doc->Sections, "Status", DPS_HTTP_STATUS_PARTIAL_OK);
	break;
      }
      Doc->Buf.allocated_size += DPS_NET_BUF_SIZE;
      if ((pfree = (Byte*)DpsRealloc(pfree, (size_t)Doc->Buf.allocated_size + 1)) == NULL) {
	inflateEnd(&zstream);
	return -1;
      }
      zstream.next_out = pfree + zstream.total_out;
      zstream.avail_out = (uLong)(Doc->Buf.allocated_size - zstream.total_out);
    } else break;
  }
  inflateEnd(&zstream);

  if(zstream.total_out == 0) {
    DPS_FREE(pfree);
    return -1;
  }
  
  DPS_FREE(Doc->Buf.buf);
  Doc->Buf.buf = (char*)pfree;
  Doc->Buf.size = gap + zstream.total_out;
  Doc->Buf.allocated_size = Doc->Buf.size + 1;

  if ((Doc->Buf.buf = (char*)DpsRealloc(Doc->Buf.buf, Doc->Buf.allocated_size)) == NULL) {
    return -1;
  }

  Doc->Buf.content = Doc->Buf.buf + gap;
  Doc->Buf.content[zstream.total_out] = '\0';

  return 0;
}


struct gztrailer {
    int crc32;
    int zlen;
};

__C_LINK int __DPSCALL DpsUnGzip(DPS_AGENT *query, DPS_DOCUMENT *Doc) {

  const unsigned char gzheader[10] = { 0x1f, 0x8b, Z_DEFLATED, 0, 0, 0, 0, 0, 0, 3 };
  Byte *cpData;
  z_stream zstream;
  Byte *buf;
  size_t csize, xlen, gap;
  int rc;
  
  if( (Doc->Buf.size) <= (Doc->Buf.content - Doc->Buf.buf + sizeof(gzheader)) )
    return -1;

  /* check magic identificator */
  if (memcmp(Doc->Buf.content, gzheader, 2) != 0) return -1;

  gap = (Doc->Buf.content - Doc->Buf.buf)/* + 1*/;
  csize = Doc->Buf.size - gap;

  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;

  inflateInit2(&zstream, -MAX_WBITS);

  buf = zstream.next_out = (Byte*)DpsMalloc((size_t)Doc->Buf.allocated_size + 1);
  if (buf == NULL) {
    inflateEnd(&zstream);
    return -1;
  }
  cpData = (Byte*)Doc->Buf.content + sizeof(gzheader);
  csize -= sizeof(gzheader);

  if (Doc->Buf.content[3] & 4) { /* FLG.FEXTRA */
    xlen = (unsigned char) cpData[1];
    xlen <<= 8; 
    xlen += (unsigned char) *cpData;
    cpData += xlen + 2;
    csize -= xlen + 2;
  }
  if (Doc->Buf.content[3] & 8) { /* FLG.FNAME */
    while (*cpData != '\0') cpData++,csize--;
    cpData++; csize--;
  }
  if (Doc->Buf.content[3] & 16) { /* FLG.FCOMMENT */
    while (*cpData != '\0') cpData++,csize--;
    cpData++; csize--;
  }
  if (Doc->Buf.content[3] & 2) { /* FLG.FHCRC */
    cpData += 2;
    csize -= 2;
  }

/*  memcpy(zstream.next_in, cpData, csize);
  zstream.next_out = (Byte*)Doc->Buf.content;*/
  dps_memmove(zstream.next_out, Doc->Buf.buf, gap);
  zstream.next_out += gap;

  zstream.next_in = cpData;
  zstream.avail_in = csize - sizeof(struct gztrailer);
  zstream.avail_out = (uLong)(Doc->Buf.allocated_size - gap);

  while(1) {
    rc = inflate(&zstream, Z_NO_FLUSH);
    if (rc == Z_OK) {
      if (Doc->Buf.allocated_size > Doc->Buf.max_size) {
	DpsLog(query, DPS_LOG_EXTRA, "Gzip: too large content");
	DpsVarListReplaceInt(&Doc->Sections, "Status", DPS_HTTP_STATUS_PARTIAL_OK);
	break;
      }
      Doc->Buf.allocated_size += DPS_NET_BUF_SIZE;
      if ((buf = (Byte*)DpsRealloc(buf, (size_t)Doc->Buf.allocated_size + 1)) == NULL) {
	inflateEnd(&zstream);
	return -1;
      }
      zstream.next_out = buf + zstream.total_out;
      zstream.avail_out = (uLong)(Doc->Buf.allocated_size - zstream.total_out);
    } else break;
  }
  inflateEnd(&zstream);

  if(zstream.total_out == 0) {
    DPS_FREE(buf);
    return -1;
  }
  
  DPS_FREE(Doc->Buf.buf);
  Doc->Buf.buf = (char*)buf;
  Doc->Buf.size = gap + zstream.total_out;
  Doc->Buf.allocated_size = Doc->Buf.size + 1;

  if ((Doc->Buf.buf = (char*)DpsRealloc(Doc->Buf.buf, Doc->Buf.allocated_size + 1)) == NULL) {
    return -1;
  }

  Doc->Buf.content = Doc->Buf.buf + gap;
  Doc->Buf.content[zstream.total_out] = '\0';
  return 0;
}

__C_LINK int __DPSCALL DpsUncompress(DPS_AGENT *query, DPS_DOCUMENT *Doc) {
  Byte *buf;
  uLong   Len;
  size_t csize, gap;
  int res;
  
  if( (Doc->Buf.size) <= (size_t)(Doc->Buf.content - Doc->Buf.buf) )
    return -1;
  gap = (Doc->Buf.content - Doc->Buf.buf)/* + 1*/;
  csize = Doc->Buf.size - gap;

  Doc->Buf.allocated_size *= 6;
  buf = (Byte*)DpsMalloc((size_t)Doc->Buf.allocated_size + 1);
  if (buf == NULL) return -1;
/*  memcpy(buf, Doc->Buf.content, csize);*/
  dps_memmove(buf, Doc->Buf.buf, gap);
  
  while(1) {
    Len = (uLong)(Doc->Buf.allocated_size - gap);
    res = uncompress(buf + gap, &Len, (Byte*)Doc->Buf.content, csize);
    if (res == Z_BUF_ERROR) {
      if (Doc->Buf.allocated_size > Doc->Buf.max_size) {
	DpsLog(query, DPS_LOG_EXTRA, "Compress: too large content");
	DpsVarListReplaceInt(&Doc->Sections, "Status", DPS_HTTP_STATUS_PARTIAL_OK);
	break;
      }
      Doc->Buf.allocated_size += DPS_NET_BUF_SIZE;
      if ((buf = (Byte*)DpsRealloc(buf, (size_t)Doc->Buf.allocated_size + 1)) == NULL) {
	return -1;
      }
    } else break;
  }
  
  DPS_FREE(Doc->Buf.buf);
  Doc->Buf.buf = (char*)buf;
  Doc->Buf.size = gap + Len;
  Doc->Buf.allocated_size = Doc->Buf.size + 1;
  if ((Doc->Buf.buf = (char*)DpsRealloc(Doc->Buf.buf, Doc->Buf.allocated_size)) == NULL) {
    return -1;
  }

  if(res != Z_OK) return -1;
  
  Doc->Buf.content = Doc->Buf.buf + gap;
  Doc->Buf.content[Len] = '\0';
  return 0;
}

#endif
