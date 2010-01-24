/* Copyright (C) 2005-2010 Datapark corp. All rights reserved.

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


#if 0
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
			DPS_TLOOP1(*dst++ = *src++);
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
			DPS_TLOOP1(*--dst = *--src);
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
#endif

#if 1

typedef long long word;  /* up to 32 bytes long */
#define wsize sizeof(word)
#define wmask (wsize - 1)

inline void dps_minimove_forward(char *dst, const char *src, size_t t) {
	if (t) { dst[0] = src[0]; 
	if (t > 1) { dst[1] = src[1]; 
	if (t > 2) { dst[2] = src[2]; 
	if (t > 3) { dst[3] = src[3]; 
	if (t > 4) { dst[4] = src[4]; 
	if (t > 5) { dst[5] = src[5]; 
	if (t > 6) { dst[6] = src[6]; 
	if (t > 7) { dst[7] = src[7]; 
	if (t > 8) { dst[8] = src[8]; 
	if (t > 9) { dst[9] = src[9]; 
	if (t > 10) { dst[10] = src[10]; 
	if (t > 11) { dst[11] = src[11]; 
	if (t > 12) { dst[12] = src[12]; 
	if (t > 13) { dst[13] = src[13]; 
	if (t > 14) { dst[14] = src[14]; 
	if (t > 15) { dst[15] = src[15]; 
	if (t > 16) { dst[16] = src[16]; 
	if (t > 17) { dst[17] = src[17]; 
	if (t > 18) { dst[18] = src[18]; 
	if (t > 19) { dst[19] = src[19]; 
	if (t > 20) { dst[20] = src[20]; 
	if (t > 21) { dst[21] = src[21]; 
	if (t > 22) { dst[22] = src[22]; 
	if (t > 23) { dst[23] = src[23]; 
	if (t > 24) { dst[24] = src[24]; 
	if (t > 25) { dst[25] = src[25]; 
	if (t > 26) { dst[26] = src[26]; 
	if (t > 27) { dst[27] = src[27]; 
	if (t > 28) { dst[28] = src[28]; 
	if (t > 29) { dst[29] = src[29]; 
	if (t > 30) { dst[30] = src[30]; 
	}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}


inline void dps_minimove_backward(char *dst, const char *src, size_t t) {
	switch(t) {
	case 31: dst[30] = src[30];
	case 30: dst[29] = src[29];
	case 29: dst[28] = src[28];
	case 28: dst[27] = src[27];
	case 27: dst[26] = src[26];
	case 26: dst[25] = src[25];
	case 25: dst[24] = src[24];
	case 24: dst[23] = src[23];
	case 23: dst[22] = src[22];
	case 22: dst[21] = src[21];
	case 21: dst[20] = src[20];
	case 20: dst[19] = src[19];
	case 19: dst[18] = src[18];
	case 18: dst[17] = src[17];
	case 17: dst[16] = src[16];
	case 16: dst[15] = src[15];
	case 15: dst[14] = src[14];
	case 14: dst[13] = src[13];
	case 13: dst[12] = src[12];
	case 12: dst[11] = src[11];
	case 11: dst[10] = src[10];
	case 10: dst[9] = src[9];
	case 9: dst[8] = src[8];
	case 8: dst[7] = src[7];
	case 7: dst[6] = src[6];
	case 6: dst[5] = src[5];
	case 5: dst[4] = src[4];
	case 4: dst[3] = src[3];
	case 3: dst[2] = src[2];
	case 2: dst[1] = src[1];
	case 1: dst[0] = src[0];
	case 0: break;
	}
}


void * dps_memcpy(void *dst0, const void *src0, size_t length) {
	size_t t;

  if (length == 0 || dst0 == src0)		/* nothing to do */
    return dst0;
  if ((DPS_PNTYPE)dst0 < (DPS_PNTYPE)src0) { /* copy forward */
    register char *dst = (char*)dst0;
    register const char *src = (const char*)src0;
    t = (unsigned int)src & wmask;
    if (t) {
    	if (length < wsize) {
		t = length;
    	} else {
		t = wsize - t;
    	}
	dps_minimove_forward(dst, src, t);
	length -= t;
	src += t; dst += t;
     }
     t = length / wsize;
     if (t) {
	register size_t n = t / 8;
    	register size_t r = (t % 8);
	register word *wdst = (word*)dst;
	register const word *wsrc = (const word*)src;
    	if (r == 0) r = 8; else n++;
    	wdst[0] = wsrc[0];
    	if (r > 1) { wdst[1] = wsrc[1];
    	if (r > 2) { wdst[2] = wsrc[2];
    	if (r > 3) { wdst[3] = wsrc[3];
    	if (r > 4) { wdst[4] = wsrc[4];
    	if (r > 5) { wdst[5] = wsrc[5];
    	if (r > 6) { wdst[6] = wsrc[6];
    	if (r > 7) { wdst[7] = wsrc[7];
	}}}}}}}
    	wsrc += r; wdst += r;
    	while (--n > 0) {
    		wdst[0] = wsrc[0];
    		wdst[1] = wsrc[1];
    		wdst[2] = wsrc[2];
    		wdst[3] = wsrc[3];
    		wdst[4] = wsrc[4];
    		wdst[5] = wsrc[5];
    		wdst[6] = wsrc[6];
    		wdst[7] = wsrc[7];
    		wsrc += 8; wdst += 8;
    	}
 	dst = (char*)wdst; src = (const char *)wsrc;
    }
    if ( (t = (length & wmask)) ) dps_minimove_forward(dst, src, t);

  } else { /* copy backward */
    register char *dst = (char*)dst0 + length;
    register const char *src = (const char *)src0 + length;
    t = (unsigned int)src & wmask;
    if (t) {
	if (length < wsize) {
		t = length;
	}
	dst -= t; src -= t;
	length -= t;
	dps_minimove_backward(dst, src, t);
    }
    t = length / wsize;
    if (t) {
    	register size_t n = t / 8;
    	register size_t r = (t % 8);
	register word *wdst = (word*)dst;
	register const word *wsrc = (const word*)src;
    	if (r == 0) r = 8; else n++;
	wsrc -= r; wdst -= r;
	switch(r) {
	case 8:wdst[7] = wsrc[7];
	case 7:wdst[6] = wsrc[6];
	case 6:wdst[5] = wsrc[5];
	case 5:wdst[4] = wsrc[4];
	case 4:wdst[3] = wsrc[3];
	case 3:wdst[2] = wsrc[2];
	case 2:wdst[1] = wsrc[1];
	case 1:wdst[0] = wsrc[0];
	}
     	while (--n > 0) {
		wsrc -= 8; wdst -= 8;
		wdst[7] = wsrc[7];
		wdst[6] = wsrc[6];
		wdst[5] = wsrc[5];
		wdst[4] = wsrc[4];
		wdst[3] = wsrc[3];
		wdst[2] = wsrc[2];
		wdst[1] = wsrc[1];
		wdst[0] = wsrc[0];
	}
	dst = (char*)wdst; src = (const char*)wsrc;
    }
    t = length & wmask;
    dps_minimove_backward(dst - t, src - t, t);
 }
  return dst0;
}



#else

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

#endif

void * dps_strcpy(void *dst0, const void *src0) {
  register char *dst = dst0;
  register const char *src = src0;
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
    register size_t n = length / 8;
    register size_t r = (length % 8);
    register char *dst = dst0;
    register const char *src = src0;
    if (r == 0) r = 8; else n++;
    if (!(dst[0] = src[0])) return dst0;
    if (r > 1) { if (!(dst[1] = src[1])) return dst0;
    if (r > 2) { if (!(dst[2] = src[2])) return dst0;
    if (r > 3) { if (!(dst[3] = src[3])) return dst0;
    if (r > 4) { if (!(dst[4] = src[4])) return dst0;
    if (r > 5) { if (!(dst[5] = src[5])) return dst0;
    if (r > 6) { if (!(dst[6] = src[6])) return dst0;
    if (r > 7) if (!(dst[7] = src[7])) return dst0;
    }}}}}}
    src += r; dst += r;
    while (--n > 0) {
      if (!(dst[0] = src[0])) break;
      if (!(dst[1] = src[1])) break;
      if (!(dst[2] = src[2])) break;
      if (!(dst[3] = src[3])) break;
      if (!(dst[4] = src[4])) break;
      if (!(dst[5] = src[5])) break;
      if (!(dst[6] = src[6])) break;
      if (!(dst[7] = src[7])) break;
      src += 8; dst += 8;
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


/*
0009 0000 0000 0000 1001
000A 0000 0000 0000 1010
000D 0000 0000 0000 1101
0020 0000 0000 0010 0000
00A0 0000 0000 1010 0000
1680 0001 0110 1000 0000
2000 0010 0000 0000 0000
2001 0010 0000 0000 0001
2002 0010 0000 0000 0010
2003 0010 0000 0000 0011
2004 0010 0000 0000 0100
2005 0010 0000 0000 0101
2006 0010 0000 0000 0110
2007 0010 0000 0000 0111
2008 0010 0000 0000 1000
2009 0010 0000 0000 1001
200A 0010 0000 0000 1010
200B 0010 0000 0000 1011
202F 0010 0000 0010 1111
2420 0010 0100 0010 0000
3000 0011 0000 0000 0000

303F 0011 0000 0011 1111
---- -------------------
CB50 11?? 1?11 ?1?1 ???? - not space bits

*/
int DpsUniNSpace(dpsunicode_t c) {
     if (c == 0x303F) return 0;
     if (c == 0xFEFF) return 0;
     if (c  & 0xCB50) return 1;
     if (c == 0x0009) return 0;
     if (c == 0x000A) return 0;
     if (c == 0x000D) return 0;
     if (c == 0x0020) return 0;
/*     if (c == 0x0026) return 0;
     if (c == 0x002C) return 0;*/
     if (c == 0x00A0) return 0;
     if (c == 0x1680) return 0;
     if ((c >= 0x2000) && (c <= 0x200B)) return 0;
     if (c == 0x202F) return 0;
     if (c == 0x2420) return 0;
     if (c == 0x3000) return 0;
     return 1;
}

