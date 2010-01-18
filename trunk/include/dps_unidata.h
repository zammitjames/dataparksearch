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

#ifndef DPS_UNIDATA_H
#define DPS_UNIDATA_H

#include "dps_config.h"

/* Character types */ 

#define DPS_UNI_UNDEF     0
#define DPS_UNI_LETTER_U  1
#define DPS_UNI_LETTER_L  2
#define DPS_UNI_LETTER_T  3
#define DPS_UNI_LETTER_M  4
#define DPS_UNI_LETTER_O  5

#define DPS_UNI_BUKVA_FORTE 5

#define DPS_UNI_SYMBOL_M  6
#define DPS_UNI_SYMBOL_C  7
#define DPS_UNI_SYMBOL_K  8
#define DPS_UNI_SYMBOL_O  9
#define DPS_UNI_NUMBER_D 10
#define DPS_UNI_NUMBER_L 11
#define DPS_UNI_NUMBER_O 12
#define DPS_UNI_MARK_N   13
#define DPS_UNI_MARK_C   14
#define DPS_UNI_MARK_E   15

#define DPS_UNI_BUKVA 15

#define DPS_UNI_SEPAR_S  16
#define DPS_UNI_SEPAR_L  17
#define DPS_UNI_SEPAR_P  18
#define DPS_UNI_PUNCT_C  19
#define DPS_UNI_PUNCT_D  20
#define DPS_UNI_PUNCT_S  21
#define DPS_UNI_PUNCT_E  22
#define DPS_UNI_PUNCT_I  23
#define DPS_UNI_PUNCT_F  24
#define DPS_UNI_PUNCT_O  25

#define DPS_UNI_OTHER_C  26
#define DPS_UNI_OTHER_F  27
#define DPS_UNI_OTHER_S  28
#define DPS_UNI_OTHER_O  29
#define DPS_UNI_OTHER_N  30

#define DPS_UNI_RAZDEL 30
#define DPS_UNI_MAX 30

#define DPS_UNI_CTYPECLASS(type)   (((type) > DPS_UNI_BUKVA) ? DPS_UNI_RAZDEL : DPS_UNI_BUKVA) 


inline extern dpsunicode_t DpsUniToLower(dpsunicode_t uni);
extern __C_LINK void __DPSCALL DpsUniStrToLower(dpsunicode_t * unistr);
extern int  DpsUniCType(dpsunicode_t u);

extern dpsunicode_t  *DpsUniGetToken(dpsunicode_t *s, dpsunicode_t ** last, int *have_bukva_forte, int loose);
extern __C_LINK dpsunicode_t  * __DPSCALL DpsUniGetSepToken(dpsunicode_t *s, dpsunicode_t ** last, int *ctype0, int *have_bukva_forte,
							    int cmd_mode);

extern dpsunicode_t *DpsUniNormalizeNFC(dpsunicode_t *buf, dpsunicode_t *str);
extern dpsunicode_t *DpsUniNormalizeNFD(dpsunicode_t *buf, dpsunicode_t *str);

extern int dps_isPatternSyntax(dpsunicode_t ch);
extern int dps_isApostropheBreak(dpsunicode_t ch, dpsunicode_t next);

#endif
