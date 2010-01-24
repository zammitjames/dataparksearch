/* Copyright (C) 2003-2010 Datapark corp. All right reserved.
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
#include <string.h>

#include "dps_sgml.h"
#include "dps_unicode.h"
#include "dps_charsetutils.h"

typedef struct {
  const char *sgml;
  int unicode;
} dps_sgml_char;

typedef int (*qsort_cmp)(const void*, const void*);

/*
static const struct dps_sgml_chars {
     const char     *sgml;
     int       unicode;
}
*/

static int dps_sgml_cmp(dps_sgml_char *s1, dps_sgml_char *s2) {
  return strcmp(s1->sgml, s2->sgml);
}

static dps_sgml_char SGMLChars[] = {

#include "sgml.inc"

/* END Marker */
/*{    "",       0    }*/
};  


dpsunicode_t DpsSgmlToUni(const char *sgml) {
/*     int i;
     int res;*/
     dps_sgml_char k, *found;
     
     k.sgml = sgml;

     found = bsearch(&k, SGMLChars, sizeof(SGMLChars) / sizeof(dps_sgml_char), sizeof(dps_sgml_char), (qsort_cmp)dps_sgml_cmp);
     if (found != NULL) return found->unicode;
     return 0;
}

/** This function replaces SGML entities
    With their character equivalents     
*/

__C_LINK char * __DPSCALL DpsSGMLUnescape(char * str){
  char *s = str,*e = str, c, z;

/*****************/     
     while(*s){
          if(*s=='&'){
               if(*(s+1)=='#'){
                    for(e=s+2;(e-s<DPS_MAX_SGML_LEN)&&(*e<='9')&&(*e>='0');e++);
                    if(*e==';'){
                         int v=atoi(s+2);
                         if(v>=0&&v<=255) {
                           *s=(char)v;
                         } else {
                           *s = ' ';
                         }
                         dps_memmove(s+1, e+1, dps_strlen(e + 1) + 1);
                    }
               }else{
                    for(e=s+1;(e-s<DPS_MAX_SGML_LEN)&&(((*e<='z')&&(*e>='a'))||((*e<='Z')&&(*e>='A')));e++);
		    z = *e;
		    *e = '\0';
                    if( (z == ';') && (c=(char)DpsSgmlToUni(s+1))){
                         *s=c;
                         dps_memmove(s+1, e+1, dps_strlen(e + 1) + 1);
                    }
		    if (z != ';') *e = z;
		    else s++;
               }
          }
          s++;
     }
     return(str);
}

/** This function replaces SGML entities
    With their UNICODE   equivalents     
*/
void DpsSGMLUniUnescape(dpsunicode_t *ustr) {
  dpsunicode_t *s = ustr, *e, c;
  char sgml[DPS_MAX_SGML_LEN+1];

  while (*s){
          if(*s=='&'){
               int i = 0;
               if(*(s+1)=='#'){
                    for(e = s + 2; (e - s < DPS_MAX_SGML_LEN) && (*e <= '9') && (*e >= '0'); e++);
                    if(*e==';'){
                         for(i = 2; s + i < e; i++)
                              sgml[i-2]=s[i];
                         sgml[i-2] = '\0';
                         *s = atoi(sgml);
                         dps_memmove(s + 1, e + 1, sizeof(dpsunicode_t) * (DpsUniLen(e + 1) + 1));
                    }
               }else{
		    for(e=s+1;(e-s<DPS_MAX_SGML_LEN)&&(((*e<='z')&&(*e>='a'))||((*e<='Z')&&(*e>='A')));e++) {
                      sgml[i] = (char)*e;
                      i++;
                    }
		    sgml[i] = '\0';
                    if( (*e==';') && (c = DpsSgmlToUni(sgml)) ) {
                         *s=c;
                         dps_memmove(s + 1, e + 1, sizeof(dpsunicode_t) * (DpsUniLen(e + 1) + 1));
                         
                    }
		    
               }
          }
          s++;
  }
}

