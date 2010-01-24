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

#include "dps_config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include "dps_unicode.h"
#include "dps_unidata.h"
#include "dps_charsetutils.h"

/*static const dpsunicode_t dps_uninullstr[] = {0};*/

/* Calculates UNICODE string length */

size_t __DPSCALL DpsUniLen(register const dpsunicode_t * u) {
#if 1
  register const dpsunicode_t *s;
  for(s = u; *s; s++);
  return (s - u);
#else
	register size_t ulen=0;
	while(*u++)ulen++;
	return(ulen);
#endif
}

/* Allocates a copy of unicode string */

dpsunicode_t *DpsUniDup(const dpsunicode_t *s) {
	dpsunicode_t *res;
	size_t size;
	
	size = (DpsUniLen(s)+1)*sizeof(*s);
	if((res=(dpsunicode_t*)DpsMalloc(size)) == NULL)
		return(NULL);
	dps_memcpy(res, s, size); /* was: dps_memmove */
	return res;
}

dpsunicode_t *DpsUniNDup(const dpsunicode_t *s, size_t len) {
	dpsunicode_t *res;
	size_t size = DpsUniLen(s);
	if (size > len) size = len;
	if((res = (dpsunicode_t*)DpsMalloc((size + 1) * sizeof(*s))) == NULL) return(NULL);
	dps_memmove(res, s, size * sizeof(*s));
	res[size] = 0;
	return res;
}

dpsunicode_t *DpsUniRDup(const dpsunicode_t *s) {
	dpsunicode_t *res;
	size_t size, len;
	
	size = ((len = DpsUniLen(s)) + 1) * sizeof(*s);
	if((res=(dpsunicode_t*)DpsMalloc(size)) == NULL)
		return(NULL);
	{
	  register size_t z;
	  size = len - 1;
	  for (z = 0; z < len; z++) res[z] = s[size - z];
	  res[len] = 0;
	}
	return res;
}

/* Compare unicode strings */

int DpsUniStrCmp(register const dpsunicode_t * s1, register const dpsunicode_t * s2) {
  while (*s1 == *s2) {
    if (*s1++ == 0)
      return (0);
    s2++;
  }
  if (*s1 < *s2) return -1;
  return 1;
}

int DpsUniStrCaseCmp(const dpsunicode_t *s1, const dpsunicode_t * s2) {
  register dpsunicode_t d1, d2;
  while ((d1 = DpsUniToLower(*s1++)) == (d2 = DpsUniToLower(*s2++))) {
    if (d1 == 0) return 0;
  }
  if (d1 < d2) return -1;
  return 1;
}


/* backward unicode string compaire */
int DpsUniStrBCmp(const dpsunicode_t *s1, const dpsunicode_t *s2) { 
  register ssize_t l1 = DpsUniLen(s1)-1, l2 = DpsUniLen(s2)-1;
  while (l1 >= 0 && l2 >= 0) {
    if (s1[l1] < s2[l2]) return -1;
    if (s1[l1] > s2[l2]) return 1;
    l1--;
    l2--;
  }
  if (l1 < l2) return -1;
  if (l1 > l2) return 1;
/*  if (*s1 < *s2) return -1;
  if (*s1 > *s2) return 1;*/
  return 0;
}

int DpsUniStrBNCmp(const dpsunicode_t *s1, const dpsunicode_t *s2, size_t count) { 
  register ssize_t l1 = DpsUniLen(s1) - 1, l2 = DpsUniLen(s2) - 1, l = count;
  while (l1 >= 0 && l2 >= 0 && l > 0) {
    if (s1[l1] < s2[l2]) return -1;
    if (s1[l1] > s2[l2]) return 1;
    l1--;
    l2--;
    l--;
  }
  if (l == 0) return 0;
  if (l1 < l2) return -1;
  if (l1 > l2) return 1;
  if (*s1 < *s2) return -1;
  if (*s1 > *s2) return 1;
  return 0;
}

/* string copy */
dpsunicode_t *DpsUniStrCpy(dpsunicode_t *dst, const dpsunicode_t *src) {
/*
  register dpsunicode_t *d = dst; register const dpsunicode_t *s = src;
  while (*s) {
    *d = *s; d++; s++;
  }
  *d = *s;
  return dst;
*/
  register size_t n = DpsUniLen(src) + 1;
  return dps_memmove(dst, src, n * sizeof(dpsunicode_t));
}

dpsunicode_t *DpsUniStrNCpy(dpsunicode_t *dst, const dpsunicode_t *src, size_t len) {
/*
  register dpsunicode_t *d = dst; register const dpsunicode_t *s = src; register size_t l = len;
  while (*s && l) {
    *d = *s; d++; s++;
    l--;
  }
  if (l) *d = *s;
  return dst;
*/
  register size_t n = DpsUniLen(src) + 1;
  return dps_memmove(dst, src, sizeof(dpsunicode_t) * ((n < len) ? n : len));
}

dpsunicode_t *DpsUniStrRCpy(dpsunicode_t *dst, const dpsunicode_t *src) {
  register size_t i = 0; dpsunicode_t *d = dst; register size_t l = DpsUniLen(src);
  if (l) for (l--; i <= l; i++)
    d[i] = src[l - i];
  d[i] = 0;
  return dst;
}

/* string append */
dpsunicode_t *DpsUniStrCat(dpsunicode_t *s, const dpsunicode_t *append) {
  size_t len = DpsUniLen(s);
  DpsUniStrCpy(&s[len], append);
  return s;
}

/* Compares two unicode strings, ignore case */
/* Not more than len characters are compared */

int DpsUniStrNCaseCmp(const dpsunicode_t *s1, const dpsunicode_t * s2, size_t len) {
  if(len != 0) {
    register dpsunicode_t d1, d2;
    do {
      d1 = DpsUniToLower(*s1);
      d2 = DpsUniToLower(*s2);
      if (d1 < d2) return -1;
      if (d1 > d2) return 1;
      if (d1 == 0) return 0;
      s1++;
      s2++;
    } while (--len != 0);
  }
  return 0;
}

int DpsUniStrNCmp(const dpsunicode_t *s1, const dpsunicode_t * s2, size_t len) {
  if(len != 0) {
    register dpsunicode_t d1, d2;
    do {
      d1 = *s1;
      d2 = *s2;
      if (d1 < d2) return -1;
      if (d1 > d2) return 1;
      if (d1 == 0) return 0;
      s1++;
      s2++;
    } while (--len != 0);
  }
  return 0;
}


dpsunicode_t *DpsUniAccentStrip(dpsunicode_t *str) {
  dpsunicode_t *nfd, *s, *d;

  s = d = nfd = DpsUniNormalizeNFD(NULL, str);
  while (*s != 0) {
    switch(DpsUniCType(*s)) {
    case DPS_UNI_MARK_N: break;
    default:
      if (s != d) *d = *s;
      d++;
    }
    s++;
  }
  *d = *s;
  return nfd;
}


dpsunicode_t *DpsUniGermanReplace(dpsunicode_t *str) {
  size_t l = DpsUniLen(str);
  dpsunicode_t *german = DpsMalloc((2 * l + 1) * sizeof(dpsunicode_t));
  if (german !=NULL) {
    dpsunicode_t *s = str, *d = german;
    while(*s) {
      switch(*s) {
      case 0x00DF: /* eszett, or scharfes s, small */
	*d++ = 's'; *d++ = 's'; break;
      case 0x1E9E: /* eszett, or scharfes s, big */
	*d++ = 'S'; *d++ = 'S'; break;
      case 0x00D6: *d++ = 'O'; *d++ = 'E'; break;
      case 0x00F6: *d++ = 'o'; *d++ = 'e'; break;

      case 0x00DC: *d++ = 'U'; *d++ = 'E'; break;
      case 0x00FC: *d++ = 'u'; *d++ = 'e'; break;

      case 0x00C4: *d++ = 'A'; *d++ = 'E'; break;
      case 0x00E4: *d++ = 'a'; *d++ = 'e'; break;

      default: *d++ = *s;
      }
      s++;
    }
    *d = 0;
  }
  return german;
}


dpsunicode_t *DpsUniStrChr(const dpsunicode_t *p, dpsunicode_t ch) {

        for (;; ++p) {
                if (*p == ch)
                        return ((dpsunicode_t *)p);
                if (*p == '\0')
                        return (NULL);
        }
        /* NOTREACHED */
}

static int dps_is_delim(const dpsunicode_t *delim, dpsunicode_t c) {
    register const dpsunicode_t *spanp;
    register dpsunicode_t sc;
    for (spanp = delim; (sc = *spanp++) != 0; ) {
	if (c == sc) {
	    return 1;
	}
    }
    return 0;
}


dpsunicode_t *DpsUniStrTok_SEA(dpsunicode_t *s, const dpsunicode_t *delim, dpsunicode_t **last) {
    const dpsunicode_t *spanp;
    dpsunicode_t *tok;

    if (s == NULL && (s = *last) == NULL) return NULL;


    while(dps_is_delim(delim, *s)) s++;
    if (*s == 0) return *last = NULL; /* no non-delimiter characters */

    tok = s;
    while(*s && !dps_is_delim(delim, *s)) s++;
    if (*s != 0) while(dps_is_delim(delim, *s)) s++;
    *last = s;
    return tok;

}
