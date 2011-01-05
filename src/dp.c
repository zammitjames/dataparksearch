/* Copyright (C) 2010 DataPark Ltd. All rights reserved.

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
#include <string.h>



#undef DPS_MEMMOVE

#include "dps_memcpy.inc"

#define DPS_MEMMOVE

#include "dps_memcpy.inc"



char *strcpy(char *dst0, const char *src0) {
  register char *dst = dst0;
  register const char *src = src0;
  while ((*dst++ = *src++));
  return dst0;
#if 0
  register size_t n = strlen(src0);
  return memmove(dst0, src0, n + 1);
#endif
}



#if 0
char * strncpy(char *dst0, const char *src0, size_t length) {
  register size_t n = dps_strlen(src0) + 1;
  return dps_memmove(dst0, src0, (n < length) ? n : length);
}

#else

typedef long long dps_strncpy_word;  /* up to 32 bytes long */
#define wsize sizeof(dps_strncpy_word)
#define wmask (wsize - 1)

#define dps_minibzero(dst, t) \
	if (t) { dst[0] = '\0'; \
	if (t > 1) { dst[1] = '\0'; \
	if (t > 2) { dst[2] = '\0'; \
	if (t > 3) { dst[3] = '\0'; \
	if (t > 4) { dst[4] = '\0'; \
	if (t > 5) { dst[5] = '\0'; \
	if (t > 6) { dst[6] = '\0'; \
	if (t > 7) { dst[7] = '\0'; \
	if (t > 8 ) { dst[8] = '\0'; \
	if (t > 9) { dst[9] = '\0'; \
	if (t > 10) { dst[10] = '\0'; \
	if (t > 11) { dst[11] = '\0'; \
	if (t > 12) { dst[12] = '\0'; \
	if (t > 13) { dst[13] = '\0'; \
	if (t > 14) { dst[14] = '\0'; \
	if (t > 15) { dst[15] = '\0'; \
	if (t > 16) { dst[16] = '\0'; \
	if (t > 17) { dst[17] = '\0'; \
	if (t > 18) { dst[18] = '\0'; \
	if (t > 19) { dst[19] = '\0'; \
	if (t > 20) { dst[20] = '\0'; \
	if (t > 21) { dst[21] = '\0'; \
	if (t > 22) { dst[22] = '\0'; \
	if (t > 23) { dst[23] = '\0'; \
	if (t > 24) { dst[24] = '\0'; \
	if (t > 25) { dst[25] = '\0'; \
	if (t > 26) { dst[26] = '\0'; \
	if (t > 27) { dst[27] = '\0'; \
	if (t > 28) { dst[28] = '\0'; \
	if (t > 29) { dst[29] = '\0'; \
	if (t > 30) { dst[30] = '\0'; \
	}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}


char * strncpy(char *dst0, const char *src0, size_t length) {
  if (length) {
    register size_t n = length / 8;
    register size_t r = (length % 8);
    register char *dst = dst0, *src = src0;
    if (r == 0) r = 8; else n++;
    if (!(dst[0] = src[0])) { dst++; src++; goto dps_strncpy_second_pas; }
    if (r > 1) { if (!(dst[1] = src[1])) { dst += 2; src += 2; goto dps_strncpy_second_pas; }
    if (r > 2) { if (!(dst[2] = src[2])) { dst += 3; src += 3; goto dps_strncpy_second_pas; }
    if (r > 3) { if (!(dst[3] = src[3])) { dst += 4; src += 4; goto dps_strncpy_second_pas; }
    if (r > 4) { if (!(dst[4] = src[4])) { dst += 5; src += 5; goto dps_strncpy_second_pas; }
    if (r > 5) { if (!(dst[5] = src[5])) { dst += 6; src += 6; goto dps_strncpy_second_pas; }
    if (r > 6) { if (!(dst[6] = src[6])) { dst += 7; src += 7; goto dps_strncpy_second_pas; }
    if (r > 7) { if (!(dst[7] = src[7])) { dst += 8; src += 8; goto dps_strncpy_second_pas; }
    }}}}}}}
    src += r; dst += r;
    while (--n > 0) {
      if (!(dst[0] = src[0])) { dst++; src++; goto dps_strncpy_second_pas; }
      if (!(dst[1] = src[1])) { dst += 2; src += 2; goto dps_strncpy_second_pas; }
      if (!(dst[2] = src[2])) { dst += 3; src += 3; goto dps_strncpy_second_pas; }
      if (!(dst[3] = src[3])) { dst += 4; src += 4; goto dps_strncpy_second_pas; }
      if (!(dst[4] = src[4])) { dst += 5; src += 5; goto dps_strncpy_second_pas; }
      if (!(dst[5] = src[5])) { dst += 6; src += 6; goto dps_strncpy_second_pas; }
      if (!(dst[6] = src[6])) { dst += 7; src += 7; goto dps_strncpy_second_pas; }
      if (!(dst[7] = src[7])) { dst += 8; src += 8; goto dps_strncpy_second_pas; }
      src += 8; dst += 8;
    }
dps_strncpy_second_pas:
    if (dst < dst0 + length) {
      size_t t, restlen = length - (dst - dst0);
      t = (unsigned int)dst & wmask;
      if (t) {
    	if (restlen < wsize) {
		t = restlen;
    	} else {
		t = wsize - t;
    	}
	dps_minibzero(dst, t);
	restlen -= t;
	dst += t;
      }
      t = restlen / wsize;
      if (t) {
	n = t / 8;
    	r = (t % 8 );
	register dps_strncpy_word *wdst = (dps_strncpy_word*)dst;
    	if (r == 0) r = 8; else n++;
    	wdst[0] = (dps_strncpy_word)0;
    	if (r > 1) { wdst[1] = (dps_strncpy_word)0;
    	if (r > 2) { wdst[2] = (dps_strncpy_word)0;
    	if (r > 3) { wdst[3] = (dps_strncpy_word)0;
    	if (r > 4) { wdst[4] = (dps_strncpy_word)0;
    	if (r > 5) { wdst[5] = (dps_strncpy_word)0;
    	if (r > 6) { wdst[6] = (dps_strncpy_word)0;
    	if (r > 7) { wdst[7] = (dps_strncpy_word)0;
	}}}}}}}
    	wdst += r;
    	while (--n > 0) {
    		wdst[0] = (dps_strncpy_word)0;
    		wdst[1] = (dps_strncpy_word)0;
    		wdst[2] = (dps_strncpy_word)0;
    		wdst[3] = (dps_strncpy_word)0;
    		wdst[4] = (dps_strncpy_word)0;
    		wdst[5] = (dps_strncpy_word)0;
    		wdst[6] = (dps_strncpy_word)0;
    		wdst[7] = (dps_strncpy_word)0;
    		wdst += 8;
    	}
 	dst = (char*)wdst;
      }
      if ( (t = (restlen & wmask)) ) {
	dps_minibzero(dst, t);
      }
    }
  }
  return dst0;
}







#if 0
char * strncpy(char *dst0, const char *src0, size_t length) {
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

#endif


char * strcat(char *dst0, const char *src0) {
  register size_t n = strlen(dst0);
  strcpy((char*)dst0 + n, src0);
  return dst0;
}


char * strncat(char *dst0, const char *src0, size_t length) {
  register size_t n = strlen(dst0);
  strncpy((char*)dst0 + n, src0, length);
  return dst0;
}


size_t strlen(const char *src) {
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

