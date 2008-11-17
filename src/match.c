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
#include "dps_utils.h"
#include "dps_wild.h"
#include "dps_match.h"
#include "dps_db.h"
#include "dps_vars.h"
#include "dps_charsetutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

#define ERRSTRSIZ 1024
/*
#define DEBUG_MATCH 1
*/

int DpsMatchComp(DPS_MATCH *Match,char *errstr,size_t errstrsize){
	
	int flag=REG_EXTENDED;
	int err;
	
	errstr[0]='\0';
	
	switch(Match->match_type){
		case DPS_MATCH_REGEX:
		        if (Match->compiled) regfree((regex_t*)Match->reg);
			Match->reg = (regex_t*)DpsRealloc(Match->reg, sizeof(regex_t));
			if (Match->reg == NULL) {
			  dps_snprintf(errstr, errstrsize, "Can't alloc for regex at %s:%d\n", __FILE__, __LINE__);
				fprintf(stderr, " !!! - regexcomp: %s\n", errstr);
			  return 1;
			}
			bzero((void*)Match->reg, sizeof(regex_t));
			if(Match->case_sense)
				flag|=REG_ICASE;
			if((err=regcomp(Match->reg, Match->pattern, flag))){
				regerror(err, Match->reg, errstr, errstrsize);
				fprintf(stderr, "DpsMatchComp of %s !!! - regexcomp[%d]: %s\n", Match->pattern, err, errstr);
				DPS_FREE(Match->reg);
				return(1);
			}
			Match->compiled = 1;
			
		case DPS_MATCH_WILD:
		case DPS_MATCH_BEGIN:
		case DPS_MATCH_END:
		case DPS_MATCH_SUBSTR:
		case DPS_MATCH_FULL:
			break;
		default:
			dps_snprintf(errstr,errstrsize,"Unknown match type '%d'",Match->match_type);
			return(1);
	}
	return 0;
}

DPS_MATCH *DpsMatchInit(DPS_MATCH *M){
	bzero((void*)M,sizeof(*M));
	return M;
}

void DpsMatchFree(DPS_MATCH * Match){
	DPS_FREE(Match->pattern);
	DPS_FREE(Match->arg);
	DPS_FREE(Match->section);
	DPS_FREE(Match->subsection);
	if(Match->reg){
		regfree((regex_t*)Match->reg);
		DPS_FREE(Match->reg);
	}
	Match->compiled = 0;
}

#define DPS_NSUBS 10


int DpsMatchExec(DPS_MATCH * Match, const char * string, const char *net_string, struct sockaddr_in *sin, 
		 size_t nparts, DPS_MATCH_PART * Parts) {
	size_t		i;
	int		res=0;
	regmatch_t	subs[DPS_NSUBS];
	char            regerrstr[ERRSTRSIZ]="";
	const char	*se;
	size_t		plen,slen;

#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif
	
/*	
	fprintf(stderr, "DpsMatchExec: '%s' -> '%s' '%s'\n",string, Match->pattern, DpsMatchTypeStr(Match->match_type));
*/	
	
	switch(Match->match_type){
		case DPS_MATCH_REGEX:
		        if (!Match->compiled) if (DPS_OK != (res = DpsMatchComp(Match, regerrstr, sizeof(regerrstr) - 1))) {
/*			  fprintf(stderr, "reg.errstr: %s\n", regerrstr);*/
			  return res;
			}
			if(nparts>DPS_NSUBS)nparts=DPS_NSUBS;
			res=regexec((regex_t*)Match->reg,string,nparts,subs,0);
/*			fprintf(stderr, "regex res: %d\n", res);*/
			if(res){
/*			  char errstr[ERRSTRSIZ];
				regerror(res, Match->reg, errstr, ERRSTRSIZ);
				fprintf(stderr, " !!! - regexcomp: %s\n", errstr);
*/
				for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			}else{
				for(i=0;i<nparts;i++){
					Parts[i].beg=subs[i].rm_so;
					Parts[i].end=subs[i].rm_eo;
				}
			}
			break;
		case DPS_MATCH_WILD:
			for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			if(Match->case_sense){
				res=DpsWildCaseCmp(string,Match->pattern);
			}else{
				res=DpsWildCmp(string,Match->pattern);
			}
			break;
		case DPS_MATCH_SUBNET:
			for(i = 0; i < nparts; i++) Parts[i].beg = Parts[i].end = -1;
			{
			  dps_uint4 net, mask, addr;
			  struct sockaddr_in NET;
			  int bits;
			  if ((sin != NULL) 
			      && ((bits = inet_net_pton(AF_INET, Match->pattern, &NET.sin_addr.s_addr,sizeof(NET.sin_addr.s_addr)))!= -1)) {
			    net = (dps_uint4)ntohl(NET.sin_addr.s_addr);
			    mask = (dps_uint4)(0xffffffffU << (32 - bits));
			    addr = (dps_uint4)ntohl(sin->sin_addr.s_addr);
			    res = !((addr & mask) == net);
#ifdef DEBUG_MATCH
			    fprintf(stderr, "Subnet.pton: addr:%x @ net/mask:%x/%x [%s] => %d\n", addr, net, mask, Match->pattern, res);
#endif
			  } else {
/*			if(Match->case_sense){
				res = DpsWildCaseCmp(net_string, Match->pattern);
			}else{*/
				res = DpsWildCmp(net_string, Match->pattern);
/*			}*/
#ifdef DEBUG_MATCH
				fprintf(stderr, "Subnet.WildCmp: %s @ %s => %d\n", net_string, Match->pattern, res);
#endif
			  }
			}
			break;
		case DPS_MATCH_BEGIN:
			for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			slen=dps_strlen(Match->pattern);
			if(Match->case_sense){
				res=strncasecmp(Match->pattern,string,slen);
			}else{
				res=strncmp(Match->pattern,string,slen);
			}
			break;
		case DPS_MATCH_FULL:
			for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			if(Match->case_sense){
				res=strcasecmp(Match->pattern,string);
			}else{
				res=strcmp(Match->pattern,string);
			}
			break;
		case DPS_MATCH_END:
			for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			plen=dps_strlen(Match->pattern);
			slen=dps_strlen(string);
			if(slen<plen){
				res=1;
				break;
			}
			se=string+slen-plen;
			if(Match->case_sense){
				res=strcasecmp(Match->pattern,se);
			}else{
				res=strcmp(Match->pattern,se);
			}
			break;

		case DPS_MATCH_SUBSTR:
		default:
			for(i=0;i<nparts;i++)Parts[i].beg=Parts[i].end=-1;
			res=0;
	}
	if (Match->nomatch) res = !res;
#ifdef WITH_PARANOIA
	DpsViolationExit(paran);
#endif
	return res;
}


int DpsMatchApply(char *res, size_t size, const char *string, const char *rpl,
			DPS_MATCH *Match, size_t nparts, DPS_MATCH_PART *Parts) {
	char		*dst;
	int		len=0;
	const char	*repl=rpl;
	char            digit[2];
	int             sub;
	size_t          len1, avail1;

	if(!size)return 0;
	
	switch(Match->match_type){
		case DPS_MATCH_REGEX:
			dst=res;

			while((*repl)&&((size_t)(dst-res)<(size-1))){
				if(*repl=='$' && repl[1] > '0' && repl[1] <= '9') {
							
					digit[0]=repl[1];
					digit[1]='\0';
					sub=atoi(digit);

					if((Parts[sub].beg>-1)&&(Parts[sub].end>Parts[sub].beg)){
						len1 = Parts[sub].end - Parts[sub].beg;
						avail1 = size - (dst - res) - 1;
						if (len1 > avail1) len1 = avail1;
						dps_strncpy(dst, string + Parts[sub].beg, len1);
						dst+=len1;
						*dst='\0';
					}
					repl+=2;
				}else{
					*dst=*repl;
					dst++;
					*dst='\0';
					repl++;
				}
			}
			*dst='\0';
			len=dst-res;
			break;
			
		case DPS_MATCH_BEGIN:
			len = dps_snprintf(res, size - 1, "%s%s", rpl, string + dps_strlen(Match->pattern));
			break;
		default:
			*res='\0';
			len=0;
			break;
	}
	return len;
}



int DpsMatchListAdd(DPS_AGENT *A, DPS_MATCHLIST *L, DPS_MATCH *M, char *err, size_t errsize, int ordre) {
	DPS_MATCH	*N;
	DPS_SERVER n;
	int rc;
	size_t i;

	for (i = 0; i < L->nmatches; i++) {
	  if ((strcmp(L->Match[i].pattern, M->pattern) == 0) &&
	      (L->Match[i].match_type == M->match_type) &&
	      (L->Match[i].case_sense == M->case_sense) &&
	      (L->Match[i].nomatch == M->nomatch)) {
	    return DPS_OK;
	  }
	}
	
	L->Match=(DPS_MATCH *)DpsRealloc(L->Match,(L->nmatches+1)*sizeof(DPS_MATCH));
	if (L->Match == NULL) {
	  L->nmatches = 0;
	  dps_snprintf(err, errsize, "Can't realloc at %s:%d\n", __FILE__, __LINE__);
	  return DPS_ERROR;
	}
	N=&L->Match[L->nmatches++];
	DpsMatchInit(N);
	N->pattern = (char*)DpsStrdup(M->pattern);
	N->match_type=M->match_type;
	N->case_sense=M->case_sense;
	N->nomatch=M->nomatch;
	N->arg = M->arg ? (char*)DpsStrdup(M->arg) : NULL;
	N->section = M->section ? (char *)DpsStrdup(M->section) : NULL;
	N->subsection = M->subsection ? (char *)DpsStrdup(M->subsection) : NULL;

	if (A != NULL) {

	  bzero((void*)&n, sizeof(n));
	  n.command = 'F';
	  n.Match.pattern = M->pattern;
	  n.Match.match_type = M->match_type;
	  n.Match.case_sense = M->case_sense;
	  n.Match.nomatch = M->nomatch;
	  n.Match.arg = N->arg;
	  n.Match.section = N->section;
	  n.Match.subsection = N->subsection;
	  n.ordre = ordre;

	  rc = DpsSrvAction(A, &n, DPS_SRV_ACTION_ADD);
	  N->server_id = n.site_id;
	  DpsVarListFree(&n.Vars);

	  if (rc != DPS_OK) return rc;
	}

	return DpsMatchComp(N, err, errsize);
}


DPS_MATCHLIST *DpsMatchListInit(DPS_MATCHLIST *L){
	bzero((void*)L,sizeof(*L));
	return L;
}

void DpsMatchListFree(DPS_MATCHLIST *L){
	size_t i;
	for(i=0;i<L->nmatches;i++){
		DpsMatchFree(&L->Match[i]);
	}
	L->nmatches=0;
	DPS_FREE(L->Match);
}

DPS_MATCH * __DPSCALL DpsMatchListFind(DPS_MATCHLIST *L, const char *str, size_t nparts, DPS_MATCH_PART *Parts) {
	size_t i;
	for(i=0;i<L->nmatches;i++){
		DPS_MATCH *M=&L->Match[i];
		if(!DpsMatchExec(M, str, str, NULL, nparts, Parts))
			return M;
	}
	return NULL;
}


DPS_MATCH * __DPSCALL DpsSectionMatchListFind(DPS_MATCHLIST *L, DPS_DOCUMENT *Doc, size_t nparts, DPS_MATCH_PART *Parts){
  size_t i, j, r;
  DPS_TEXTLIST	*tlist = &Doc->TextList;

	for(i = 0; i < L->nmatches; i++) {
		DPS_MATCH *M = &L->Match[i];
/*		const char *str = DpsVarListFindStr(&Doc->Sections, M->section, "");*/
		
		if (M->section) {
		  r = (size_t)dps_tolower(M->section[0]);
		  for(j = 0; j < Doc->Sections.Root[r].nvars; j++) {
		    DPS_VAR *Sec = &Doc->Sections.Root[r].Var[j];
		    if(/*Sec->section &&*/ Sec->val != NULL && !strcasecmp(M->section, Sec->name)) {
		      if(!DpsMatchExec(M, Sec->val, Sec->val, NULL, nparts, Parts)) return M;
		    }
		  }
		}

		for(j = 0; j < tlist->nitems; j++) {
		  DPS_TEXTITEM	*Item = &tlist->Items[j];
		  if (Item->section && !strcasecmp(DPS_NULL2EMPTY(M->section), DPS_NULL2EMPTY(Item->section_name))) {
		    if(!DpsMatchExec(M, Item->str, Item->str, NULL, nparts, Parts)) return M;
		  }
		}
	}
	return NULL;
}


const char *DpsMatchTypeStr(int m){
	switch(m){
		case DPS_MATCH_REGEX:	return	"Regex";
		case DPS_MATCH_WILD:	return	"Wild";
		case DPS_MATCH_BEGIN:	return	"Begin";
		case DPS_MATCH_END:	return	"End";
		case DPS_MATCH_SUBSTR:	return	"SubStr";
		case DPS_MATCH_FULL:	return	"Full";
		case DPS_MATCH_SUBNET:	return	"Subnet";
	}
	return "<Unknown Match Type>";
}
