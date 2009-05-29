/* Copyright (C) 2005-2009 Datapark corp. All rights reserved.

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

#include "dps_config.h"
#include "dps_charsetutils.h"
#include "dps_unidata.h"
#include "dps_xmalloc.h"
#include "dps_unicode.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>


__C_LINK int dps_tolower(int c) {
  if (c < 0x80) return (int)DpsUniToLower((dpsunicode_t)c);
  return tolower(c);
}


/***** dps_memmove ****/

/*
 * sizeof(dps_word) MUST BE A POWER OF TWO SO THAT dps_wmask BELOW IS ALL ONES
 */
#define dps_wsize sizeof(dps_word)
#define dps_wmask (dps_wsize - 1)

void * dps_memmove(void *dst0, const void *src0, size_t length) {
	char *dst = dst0;
	const char *src = src0;
	size_t t;

	if (length == 0 || dst == src)		/* nothing to do */
		goto done;

	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define	DPS_TLOOP(s) if (t) DPS_TLOOPo(s)
#define	DPS_TLOOP1(s) do { s; } while (--t)
#define DPS_TLOOPo(s)			  \
	{ \
		register size_t n = (t + 7) / 8; \
		switch(t % 8) {           \
		case 0:	do {	s;        \
		case 7:		s;        \
		case 6:		s;        \
		case 5:		s;        \
		case 4:		s;        \
		case 3:		s;        \
		case 2:		s;        \
		case 1:		s;        \
			} while(--n > 0); \
		}                         \
	}

	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (dps_uintptr_t)src;	/* only need low bits */
		if ((t | (dps_uintptr_t)dst) & dps_wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (dps_uintptr_t)dst) & dps_wmask || length < dps_wsize)
				t = length;
			else
				t = dps_wsize - (t & dps_wmask);
			length -= t;
			DPS_TLOOPo(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / dps_wsize;
		DPS_TLOOP(*(dps_word *)dst = *(dps_word *)src; src += dps_wsize; dst += dps_wsize);
		t = length & dps_wmask;
		DPS_TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t & dps_wmask) bytes to align, not dps_wsize - (t & dps_wmask).
		 */
		src += length;
		dst += length;
		t = (dps_uintptr_t)src;
		if ((t | (dps_uintptr_t)dst) & dps_wmask) {
			if ((t ^ (dps_uintptr_t)dst) & dps_wmask || length <= dps_wsize)
				t = length;
			else
				t &= dps_wmask;
			length -= t;
			DPS_TLOOPo(*--dst = *--src);
		}
		t = length / dps_wsize;
		DPS_TLOOP(src -= dps_wsize; dst -= dps_wsize; *(dps_word *)dst = *(dps_word *)src);
		t = length & dps_wmask;
		DPS_TLOOP(*--dst = *--src);
	}
done:
	return (dst0);
}

/**** /dps_memmove ****/


void * dps_memcpy(void *dst0, const void *src0, size_t length) {

  if (length == 0 || dst0 == src0)		/* nothing to do */
    return dst0;
  if ((unsigned long)dst0 < (unsigned long)src0) { /* copy forward */
    register size_t n = (length + 7) / 8;
    register char *dst = dst0, *src = src0;
    switch(length % 8) {
    case 0:	do {	*dst++ = *src++;
    case 7:		*dst++ = *src++;
    case 6:		*dst++ = *src++;
    case 5:		*dst++ = *src++;
    case 4:		*dst++ = *src++;
    case 3:		*dst++ = *src++;
    case 2:		*dst++ = *src++;
    case 1:		*dst++ = *src++;
                    } while(--n > 0);
    }
  } else { /* copy backward */
    register size_t n = (length + 7) / 8;
    register char *dst = dst0 + length, *src = src0 + length;
    switch(length % 8) {
    case 0:	do {	*--dst = *--src;
    case 7:		*--dst = *--src;
    case 6:		*--dst = *--src;
    case 5:		*--dst = *--src;
    case 4:		*--dst = *--src;
    case 3:		*--dst = *--src;
    case 2:		*--dst = *--src;
    case 1:		*--dst = *--src;
                    } while(--n > 0);
    }
  }
  return dst0;
}

void * dps_strcpy(void *dst0, const void *src0) {
  register char *dst = dst0, *src = src0;
  while ((*dst++ = *src++));
  return dst0;
#if 0
  register size_t n = dps_strlen(src0);
  return dps_memmove(dst0, src0, n + 1);
#endif
}

#if 0
void * dps_strncpy(void *dst0, const void *src0, size_t length) {
  register size_t n = dps_strlen(src0) + 1;
  return dps_memmove(dst0, src0, (n < length) ? n : length);
}

#else

void * dps_strncpy(void *dst0, const void *src0, size_t length) {
  if (length) {
    register size_t n = (length + 7) / 8;
    register char *dst = dst0, *src = src0;
    switch(length % 8) {
    case 0:	do {	if (!(*dst++ = *src++)) break;
    case 7:		if (!(*dst++ = *src++)) break;
    case 6:		if (!(*dst++ = *src++)) break;
    case 5:		if (!(*dst++ = *src++)) break;
    case 4:		if (!(*dst++ = *src++)) break;
    case 3:		if (!(*dst++ = *src++)) break;
    case 2:		if (!(*dst++ = *src++)) break;
    case 1:		if (!(*dst++ = *src++)) break;
                 } while(--n > 0);
    }
  }
  return dst0;
}
#endif

void * dps_strcat(void *dst0, const void *src0) {
  register size_t n = dps_strlen(dst0);
  dps_strcpy((char*)dst0 + n, src0);
  return dst0;
}

void * dps_strncat(void *dst0, const void *src0, size_t length) {
  register size_t n = dps_strlen(dst0);
  dps_strncpy((char*)dst0 + n, src0, length);
  return dst0;
}

size_t dps_strlen(const char *src) {
#if 1
  register const char *s = src;

  if (s) for (; *s; s++);
  return(s - src);

#else
    size_t len = 0;
    switch(
#if SIZEOF_CHARP == 4
	   (dps_uint4)src & 3
#else
	   (dps_uint8)src & 3
#endif
	   ) {
    case 1: if (*src++ == 0) return len; len++;
    case 2: if (*src++ == 0) return len; len++;
    case 3: if (*src++ == 0) return len; len++;
    default:
      for(;;) {
        dps_uint4 x = *(dps_uint4*)src;
#ifdef WORDS_BIGENDIAN
        if((x & 0xFF000000) == 0) return len;
        if((x & 0xFF0000) == 0) return len + 1;
        if((x & 0xFF00) == 0) return len + 2;
        if((x & 0xFF) == 0) return len + 3;
#else
        if((x & 0xFF) == 0) return len;
        if((x & 0xFF00) == 0) return len + 1;
        if((x & 0xFF0000) == 0) return len + 2;
        if((x & 0xFF000000) == 0) return len + 3;
#endif
        src += 4, len += 4;
      }
    }
    /* this point is reachless. possible compilation warning is OK */
#endif
}

void dps_mstr(char *s, const char *src, size_t l1, size_t l2) {
  l1 = l1 < l2 ? l1 : l2;
  dps_memmove(s, src, l1);
  s[l1] = '\0';
}


/* <DSTR> */

DPS_DSTR *DpsDSTRInit(DPS_DSTR *dstr, size_t page_size) {
        if (page_size == 0) return NULL;

        if (dstr == NULL) {
                dstr = DpsXmalloc(sizeof(DPS_DSTR));
                if (! dstr) return(NULL);
                dstr->freeme = 1;
        } else {
                dstr->freeme = 0;
        }

        dstr->data = DpsXmalloc(page_size);
        if (dstr->data == NULL) {
                if (dstr->freeme) DpsFree(dstr);
                return NULL;
        }
        dstr->page_size = page_size;
        dstr->data_size = 0;
        dstr->allocated_size = page_size;
        return dstr;
}

void DpsDSTRFree(DPS_DSTR *dstr) {
        if (dstr->data) DpsFree(dstr->data);
	if (dstr->freeme) DpsFree(dstr);
}

size_t DpsDSTRAppend(DPS_DSTR *dstr, const void *data, size_t append_size) {
        size_t bytes_left = (dstr->allocated_size - dstr->data_size);
        size_t asize;
	char *dstr_data;

        if (data == NULL || append_size == 0) return 0;

        if (bytes_left <= append_size + 2 * sizeof(dpsunicode_t)) {
	  asize = dstr->allocated_size + ((append_size - bytes_left) / dstr->page_size + 1) * dstr->page_size + 3 * sizeof(dpsunicode_t);
	  dstr->data = DpsRealloc(dstr->data, asize);
	  if (dstr->data == NULL) { dstr->allocated_size = dstr->data_size = 0; return 0; }
	  dstr->allocated_size = asize;
        }
	dstr_data = dstr->data;
        dps_memcpy(dstr_data + dstr->data_size, data, append_size);  /* was: dps_memmove */
        dstr->data_size += append_size;
	dstr_data += dstr->data_size;
	bzero(dstr_data, 2 * sizeof(dpsunicode_t));
        return append_size;
}

size_t DpsDSTRAppendStr(DPS_DSTR *dstr, const char *data) {
  return DpsDSTRAppend(dstr, data, dps_strlen(data));
}

size_t DpsDSTRAppendStrWithSpace(DPS_DSTR *dstr, const char *data) {
  char space[] = { 0x20, 0 };
  size_t rc;
/*  fprintf(stderr, "size:%d Append:%s|\n", dstr->data_size, data);*/
  rc = (dstr->data_size) ? DpsDSTRAppend(dstr, space, 1) : 0;
  rc += DpsDSTRAppend(dstr, data, dps_strlen(data));
  return rc;
}

size_t DpsDSTRAppendUniStr(DPS_DSTR *dstr, const dpsunicode_t *data) {
  return DpsDSTRAppend(dstr, data, sizeof(dpsunicode_t) * DpsUniLen(data) );
}

size_t DpsDSTRAppendUni(DPS_DSTR *dstr, const dpsunicode_t data) {
  register dpsunicode_t *dstr_data;
  if (dstr->data_size + sizeof(dpsunicode_t) >= dstr->allocated_size) {
    if ((dstr->data = DpsRealloc(dstr->data, dstr->allocated_size += dstr->page_size)) == NULL) {
      dstr->data_size = dstr->allocated_size = 0;
      return 0;
    }
  }
  dstr_data = (dpsunicode_t*)dstr->data;
  dstr_data[ dstr->data_size / sizeof(dpsunicode_t) ] = data;
  dstr->data_size += sizeof(dpsunicode_t);
  
  return sizeof(dpsunicode_t);
}

size_t DpsDSTRAppendUniWithSpace(DPS_DSTR *dstr, const dpsunicode_t *data) {
  dpsunicode_t space[] = {0x20, 0 };
  size_t rc;
  rc = (dstr->data_size) ? DpsDSTRAppend(dstr, space, sizeof(dpsunicode_t)) : 0;
  rc += DpsDSTRAppend(dstr, data, sizeof(dpsunicode_t) * DpsUniLen(data) );
  return rc;
}



/* </DSTR> */
