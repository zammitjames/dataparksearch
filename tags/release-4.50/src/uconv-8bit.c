/* Copyright (C) 2003-2007 Datapark corp. All rights reserved.
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
#include "dps_uniconv.h"
#include "dps_unidata.h"
#include "dps_sgml.h"
#include "dps_charsetutils.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

__C_LINK int __DPSCALL dps_mb_wc_8bit(DPS_CONV *conv, DPS_CHARSET *cs, dpsunicode_t *wc,
				      const unsigned char *str, const unsigned char *end) {

  const unsigned char *p;
  unsigned char *e, z;
  unsigned int sw;

  conv->ocodes = 1;

  if ( (*str == '&' && ((conv->flags & DPS_RECODE_HTML_FROM)||(conv->flags & DPS_RECODE_URL_FROM)) ) || 
       (*str == '!' && (conv->flags & DPS_RECODE_URL_FROM)) ) {
/*    p = strchr(str, ';');*/
    /*if (p != NULL)*/ {
      if (str[1] == '#') {
	p = str + 2;
	if (str[2] == 'x' || str[2] == 'X') sscanf(str + 3, "%x", &sw);
	else sscanf(str + 2, "%d", &sw);
	*wc = (dpsunicode_t)sw;
	if (sw < 256 && sw > 0x20 && DpsUniCType(*wc) >= DPS_UNI_OTHER_C) { // try to resolve bogus ENTITY escaping
	  dpsunicode_t sv = cs->tab_to_uni[sw];
	  if (DpsUniCType(sv) < DPS_UNI_OTHER_C) *wc = sv;
	}
      } else {
	p = str + 1;
	if (!(conv->flags & DPS_RECODE_TEXT_FROM)) {
	  for(e = str + 1 ; (e - str < DPS_MAX_SGML_LEN) && (((*e<='z')&&(*e>='a'))||((*e<='Z')&&(*e>='A'))); e++);
	  if (/*!(conv->flags & DPS_RECODE_URL_FROM) ||*/ (*e == ';')) {
	    z = *e;
	    *e = '\0';
	    *wc = DpsSgmlToUni(str + 1);
	    *e = z;
	  } else *wc = 0;
	} else *wc = 0;
      }
      if (*wc) {
	for (; isalpha(*p) || isdigit(*p); p++);
	if (*p == ';') p++;
	return conv->icodes = (p - str /*+ 1*/);
      }
    }
  }

  conv->icodes = 1;
  *wc = cs->tab_to_uni[*str];
  return (!wc[0] && str[0]) ? DPS_CHARSET_ILSEQ : 1;
}

__C_LINK int __DPSCALL dps_wc_mb_8bit(DPS_CONV *conv, DPS_CHARSET *cs, const dpsunicode_t *wc, unsigned char *s, unsigned char *e) {
     DPS_UNI_IDX *idx;
     
  conv->icodes = conv->ocodes = 1;

     for(idx=cs->tab_from_uni; idx->tab ; idx++){
          if(idx->from <= *wc && idx->to >= *wc){
               s[0]=idx->tab[*wc - idx->from];
               if ((conv->flags & DPS_RECODE_HTML_TO) && (strchr(DPS_NULL2EMPTY(conv->CharsToEscape), (int)s[0]) != NULL))
                 return DPS_CHARSET_ILUNI;
               if ((conv->flags & DPS_RECODE_URL_TO) && (s[0] == '!')) 
                 return DPS_CHARSET_ILUNI;
               return (!s[0] && *wc) ? DPS_CHARSET_ILUNI : 1;
          }
     }
     return DPS_CHARSET_ILUNI;
}
