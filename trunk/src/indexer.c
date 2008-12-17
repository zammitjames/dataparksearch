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

#include "dps_common.h"
#include "dps_utils.h"
#include "dps_log.h"
#include "dps_conf.h"
#include "dps_indexer.h"
#include "dps_indexertool.h"
#include "dps_robots.h"
#include "dps_db.h"
#include "dps_sqldbms.h"
#include "dps_url.h"
#include "dps_parser.h"
#include "dps_proto.h"
#include "dps_hrefs.h"
#include "dps_mutex.h"
#include "dps_hash.h"
#include "dps_xmalloc.h"
#include "dps_http.h"
#include "dps_host.h"
#include "dps_server.h"
#include "dps_alias.h"
#include "dps_word.h"
#include "dps_crossword.h"
#include "dps_parsehtml.h"
#include "dps_parsexml.h"
#include "dps_spell.h"
#include "dps_execget.h"
#include "dps_agent.h"
#include "dps_match.h"
#include "dps_doc.h"
#include "dps_result.h"
#include "dps_parsedate.h"
#include "dps_unicode.h"
#include "dps_unidata.h"
#include "dps_uniconv.h"
#include "dps_contentencoding.h"
#include "dps_vars.h"
#include "dps_guesser.h"
#include "dps_textlist.h"
#include "dps_id3.h"
#include "dps_stopwords.h"
#include "dps_wild.h"
#include "dps_image.h"
#include "dps_charsetutils.h"
#include "dps_template.h"
#ifdef HAVE_ZLIB
#include "dps_store.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <assert.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_LIBUTIL_H
#include <libutil.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef   HAVE_UNISTD_H
#include <unistd.h>
#endif

     #include <sys/mman.h>

/* This should be last include */
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define DPS_THREADINFO(A,s,m)	if(A->Conf->ThreadInfo)A->Conf->ThreadInfo(A,s,m)

/***************************************************************************/

#define MAXHSIZE	8192 /*4096*/	/* TUNE */

#define NS 10
static int DpsFilterFind(int log_level, DPS_MATCHLIST *L, const char *newhref, char *reason, int default_method) {
	DPS_MATCH_PART	P[NS];
	DPS_MATCH	*M;
	int		res = default_method;
	
#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif
	if( (default_method != DPS_METHOD_DISALLOW) && (M = DpsMatchListFind(L, newhref, NS, P))) {
	  if (DpsNeedLog(log_level))
	    dps_snprintf(reason, PATH_MAX, "%s %s %s '%s'", DPS_NULL2EMPTY(M->arg), DpsMatchTypeStr(M->match_type),
			 M->case_sense ? "Sensitive" : "InSensitive", M->pattern);
	  res = DpsMethod(M->arg);
	  switch(default_method) {
	  case DPS_METHOD_HEAD: 
	  case DPS_METHOD_HREFONLY: if (res == DPS_METHOD_GET) res = default_method; break;
	  case DPS_METHOD_VISITLATER: if (res != DPS_METHOD_DISALLOW) res = default_method; break;
	  }
	}else{
	  if (DpsNeedLog(log_level))
	    sprintf(reason, "%s by default", DpsMethodStr(default_method));
	}
#ifdef WITH_PARANOIA
	DpsViolationExit(-1, paran);
#endif
	return res;
}

static int DpsSectionFilterFind(int log_level, DPS_MATCHLIST *L, DPS_DOCUMENT *Doc, char *reason){
	DPS_MATCH_PART	P[NS];
	DPS_MATCH	*M;
	int		res = DPS_METHOD_UNKNOWN;
	
#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif

	if((M = DpsSectionMatchListFind(L, Doc, NS, P))) {
	  if (DpsNeedLog(log_level))
	    dps_snprintf(reason, PATH_MAX, "%s %s %s '%s'", M->arg, DpsMatchTypeStr(M->match_type), 
			 M->case_sense ? "Sensitive" : "InSensitive", M->pattern);
	  res = DpsMethod(M->arg);
	}else{
	  if (DpsNeedLog(log_level))
	    dps_snprintf(reason, PATH_MAX, "%s method is used", DpsMethodStr(Doc->method));
	}
#ifdef WITH_PARANOIA
	DpsViolationExit(-1, paran);
#endif
	return res;
}

static int DpsStoreFilterFind(int log_level, DPS_MATCHLIST *L, DPS_DOCUMENT *Doc, char *reason) {
	DPS_MATCH_PART	P[NS];
	DPS_MATCH	*M;
	int		res = DPS_METHOD_STORE;
	
#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif

	if((M = DpsSectionMatchListFind(L, Doc, NS, P))) {
	  if (DpsNeedLog(log_level))
	    dps_snprintf(reason, PATH_MAX, "%s %s %s '%s'", M->arg, DpsMatchTypeStr(M->match_type), 
			 M->case_sense ? "Sensitive" : "InSensitive", M->pattern);
	  res = DpsMethod(M->arg);
	}else{
	  if (DpsNeedLog(log_level))
	    sprintf(reason, "Store by default");
	}
#ifdef WITH_PARANOIA
	DpsViolationExit(-1, paran);
#endif
	return res;
}

static int DpsSubSectionMatchFind(int log_level, DPS_MATCHLIST *L, DPS_DOCUMENT *Doc, char *reason, char **subsection) {
	DPS_MATCH_PART	P[NS];
	DPS_MATCH	*M;
	int		res = DPS_METHOD_UNKNOWN;
	
#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif

	if((M = DpsSectionMatchListFind(L, Doc, NS, P))) {
	  if (DpsNeedLog(log_level))
	    dps_snprintf(reason, PATH_MAX, "%s %s %s '%s'", M->arg, DpsMatchTypeStr(M->match_type), 
			 M->case_sense ? "Sensitive" : "InSensitive", M->pattern);
	  res = DpsMethod(M->arg);
	  *subsection = M->subsection;
	  DpsVarListReplaceInt(&Doc->Sections, "Server_id", M->server_id);
	}else{
	  if (DpsNeedLog(log_level))
	    sprintf(reason, "No conditional subsection detected");
	  *subsection = NULL;
	}
#ifdef WITH_PARANOIA
	DpsViolationExit(-1, paran);
#endif
	return res;
}


int DpsHrefCheck(DPS_AGENT *Indexer, DPS_HREF *Href, const char *newhref) {
	char		reason[PATH_MAX+1]="";
	DPS_URL		*newURL;
	DPS_ROBOT_RULE	*rule;
	DPS_SERVER	*Srv;
	size_t          depth;
	const char      *method, *s;

	if ((newURL = DpsURLInit(NULL)) == NULL) {
	  return DPS_ERROR;
	}
	DpsURLParse(newURL, newhref);
	
	Href->site_id = 0;
	Href->checked = 1;

	if (!strcasecmp(DPS_NULL2EMPTY(newURL->schema), "mailto") 
	    || !strcasecmp(DPS_NULL2EMPTY(newURL->schema), "javascript")
	    || !strcasecmp(DPS_NULL2EMPTY(newURL->schema), "feed")
	    ) {
		DpsLog(Indexer,DPS_LOG_DEBUG,"'%s' schema, skip it", newURL->schema, newhref);
		Href->method=DPS_METHOD_DISALLOW;
		goto check_ret;
	}
	
	/* Check Allow/Disallow/CheckOnly stuff */
	Href->method = DpsFilterFind(DPS_LOG_DEBUG, &Indexer->Conf->Filters, newhref, reason, DPS_METHOD_GET);
	if(Href->method == DPS_METHOD_DISALLOW) {
		DpsLog(Indexer, DPS_LOG_DEBUG, "%s, skip it", reason);
		goto check_ret;
	}else{
		DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
	}

	if (!(Indexer->flags & DPS_FLAG_FAST_HREF_CHECK)) {
	  if(!(Srv = DpsServerFind(Indexer, 0, newhref, newURL->charset_id, NULL))) {
		DpsLog(Indexer, DPS_LOG_DEBUG, "no Server, skip it");
		Href->method = DPS_METHOD_DISALLOW;
		goto check_ret;
	  }
	
	  DpsLog(Indexer, DPS_LOG_DEBUG, " Server applied: site_id: %d URL: %s", Srv->site_id, Srv->Match.pattern);
	
	  method = DpsVarListFindStr(&Srv->Vars, "Method", "Allow");
	  if((Href->method = DpsMethod(method)) != DPS_METHOD_DISALLOW) {
	    /* Check Allow/Disallow/CheckOnly stuff */
	    Href->method = DpsFilterFind(DPS_LOG_DEBUG, &Indexer->Conf->Filters, newhref, reason, Href->method);
	    DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
	  }
	
	  if(Href->method == DPS_METHOD_DISALLOW) {
	    DpsLog(Indexer, DPS_LOG_DEBUG, "Disallowed by Server/Realm/Disallow command, skip it");
	    goto check_ret;
	  }

	  if(Href->method == DPS_METHOD_VISITLATER) {
	    DpsLog(Indexer, DPS_LOG_DEBUG, "Visit later by Server/Realm/Skip command, skip it");
	    goto check_ret;
	  }

	  if (Href->hops > Srv->MaxHops) {
	    DpsLog(Indexer, DPS_LOG_DEBUG, "too many hops (%d, max: %d), skip it", Href->hops, Srv->MaxHops);
	    Href->method = DPS_METHOD_DISALLOW;
	    goto check_ret;
	  }

	  depth = 0;
	  for(s = strchr(newURL->path, (int)'/'); s != NULL; s = strchr(++s, (int)'/')) depth++;
	  if (depth > Srv->MaxDepth) {
	    DpsLog(Indexer, DPS_LOG_DEBUG, "too deep depth (%d, max: %d), skip it", depth, Srv->MaxDepth);
	    Href->method = DPS_METHOD_DISALLOW;
	    goto check_ret;
	  }

	  if (Srv->use_robots) {
	    rule = DpsRobotRuleFind(Indexer, Srv, NULL, newURL, 0, 0);
	    if (rule) {
	      DpsLog(Indexer, DPS_LOG_DEBUG, "Href.robots.txt: '%s %s'", (rule->cmd==DPS_METHOD_DISALLOW)?"Disallow":"Allow", rule->path);
	      if ((rule->cmd == DPS_METHOD_DISALLOW) || (rule->cmd == DPS_METHOD_VISITLATER) ) {
		Href->method = rule->cmd;
		goto check_ret;
	      }
	    }
	  }
	  Href->server_id = Srv->site_id;
	}
 check_ret:
	DpsURLFree(newURL);
	return DPS_OK;
}

__C_LINK int __DPSCALL DpsStoreHrefs(DPS_AGENT * Indexer) {
	size_t		i, res;
	DPS_HREF        *H;
	DPS_DOCUMENT	Doc;

	DpsDocInit(&Doc);

	if (Indexer->Flags.collect_links) {
	  for (i = 0; i < Indexer->Hrefs.dhrefs; i++) {
	    H = &Indexer->Hrefs.Href[i];
	    if(H->stored == 0) {
	      if (!H->checked) DpsHrefCheck(Indexer, H, H->url);
	      if( H->method != DPS_METHOD_DISALLOW 
		  && H->method != DPS_METHOD_VISITLATER
		  /*dps_strlen(H->url) <= DPS_URLSIZE*/) {/* FIXME: replace this by config parameter chacking */
				DpsVarListReplaceInt(&Doc.Sections, "Referrer-ID", H->referrer);
				DpsVarListReplaceUnsigned(&Doc.Sections,"Hops", H->hops);
				DpsVarListReplaceStr(&Doc.Sections, "URL",H->url?H->url:"");
				DpsVarListReplaceInt(&Doc.Sections, "Site_id", H->site_id);
				DpsVarListReplaceInt(&Doc.Sections, "Server_id", H->server_id);
				DpsVarListReplaceDouble(&Doc.Sections, "weight", (double)H->weight);
				DpsVarListDel(&Doc.Sections, "E_URL");
				DpsVarListDel(&Doc.Sections, "URL_ID");
				Doc.charset_id = H->charset_id;
				if(DPS_OK != (res = DpsURLAction(Indexer, &Doc, DPS_URL_ACTION_ADD_LINK))){
				        DpsDocFree(&Doc);
					return(res);
				}
	      }
	      H->stored=1;
	    }
	  }
	}
	for (i = Indexer->Hrefs.dhrefs; i < Indexer->Hrefs.nhrefs; i++) {
	  H = &Indexer->Hrefs.Href[i];
	  if(H->stored == 0) {
	    if (!H->checked) DpsHrefCheck(Indexer, H, H->url);
	    if( H->method != DPS_METHOD_DISALLOW 
		&& H->method != DPS_METHOD_VISITLATER
		/*dps_strlen(H->url) <= DPS_URLSIZE*/) { /* FIXME: replace this by config parameter chacking */
				DpsVarListReplaceInt(&Doc.Sections, "Referrer-ID", H->referrer);
				DpsVarListReplaceUnsigned(&Doc.Sections,"Hops", H->hops);
				DpsVarListReplaceStr(&Doc.Sections,"URL",H->url?H->url:"");
				DpsVarListReplaceInt(&Doc.Sections,"Site_id", H->site_id);
				DpsVarListReplaceInt(&Doc.Sections,"Server_id", H->server_id);
				DpsVarListReplaceDouble(&Doc.Sections, "weight", (double)H->weight);
				DpsVarListDel(&Doc.Sections, "E_URL");
				DpsVarListDel(&Doc.Sections, "URL_ID");
				Doc.charset_id = H->charset_id;
				if(DPS_OK != (res = DpsURLAction(Indexer, &Doc, DPS_URL_ACTION_ADD))){
				        DpsDocFree(&Doc);
					return(res);
				}
	    }
	    H->stored=1;
	  }
	}
	DpsDocFree(&Doc);
	
	/* Remember last stored URL num */
	/* Note that it will became 0   */
	/* after next sort in AddUrl    */
	Indexer->Hrefs.dhrefs = Indexer->Hrefs.nhrefs;
	
	/* We should not free URL list with onw database */
	/* to avoid double indexing of the same document */
	/* So, do it if compiled with SQL only           */
	
	/* FIXME: this is incorrect with both SQL and built-in compiled */
	if(Indexer->Hrefs.nhrefs > MAXHSIZE)
		DpsHrefListFree(&Indexer->Hrefs);

	return DPS_OK;
}

static void RelLink(DPS_AGENT *Indexer, DPS_URL *curURL, DPS_URL *newURL, char **str) {
	const char	*schema = newURL->schema ? newURL->schema : curURL->schema;
	const char	*hostinfo = newURL->hostinfo ? newURL->hostinfo : curURL->hostinfo;
	const char	*path = (newURL->path && newURL->path[0]) ? newURL->path : curURL->path;
	const char	*fname = ((newURL->filename && newURL->filename[0]) || (newURL->path && newURL->path[0])) ? 
	  newURL->filename : curURL->filename;
	const char     *query_string = newURL->query_string;
	char		*pathfile = (char*)DpsMalloc(dps_strlen(DPS_NULL2EMPTY(path)) + dps_strlen(DPS_NULL2EMPTY(fname)) +
						     dps_strlen(DPS_NULL2EMPTY(query_string)) + 5);
	int             cascade;
	DPS_MATCH	*Alias;
	char		*alias = NULL;
	size_t		aliassize, nparts = 10;
	DPS_MATCH_PART	Parts[10];

	if (newURL->hostinfo == NULL) newURL->charset_id = curURL->charset_id;
	
	if (pathfile == NULL) return;
/*	sprintf(pathfile, "/%s%s%s",  DPS_NULL2EMPTY(path), DPS_NULL2EMPTY(fname), DPS_NULL2EMPTY(query_string));*/
	pathfile[0] = '/'; 
	dps_strcpy(pathfile + 1, DPS_NULL2EMPTY(path)); dps_strcat(pathfile, DPS_NULL2EMPTY(fname)); dps_strcat(pathfile, DPS_NULL2EMPTY(query_string));
		
	DpsURLNormalizePath(pathfile);

	if (!strcasecmp(DPS_NULL2EMPTY(schema), "mailto") 
	    || !strcasecmp(DPS_NULL2EMPTY(schema), "javascript")
	    || !strcasecmp(DPS_NULL2EMPTY(schema), "feed")
	    ) {
	        *str = (char*)DpsMalloc(dps_strlen(DPS_NULL2EMPTY(schema)) + dps_strlen(DPS_NULL2EMPTY(newURL->specific)) + 4);
		if (*str == NULL) return;
/*		sprintf(*str, "%s:%s", DPS_NULL2EMPTY(schema), DPS_NULL2EMPTY(newURL->specific));*/
		dps_strcpy(*str, DPS_NULL2EMPTY(schema)); dps_strcat(*str, ":"); dps_strcat(*str, DPS_NULL2EMPTY(newURL->specific));
	} else if(/*!strcasecmp(DPS_NULL2EMPTY(schema), "file") ||*/ !strcasecmp(DPS_NULL2EMPTY(schema), "htdb")) {
	        *str = (char*)DpsMalloc(dps_strlen(DPS_NULL2EMPTY(schema)) + dps_strlen(pathfile) + 4);
		if (*str == NULL) return;
/*		sprintf(*str, "%s:%s", DPS_NULL2EMPTY(schema), pathfile);*/
		dps_strcpy(*str, DPS_NULL2EMPTY(schema)); dps_strcat(*str, ":"); dps_strcat(*str, pathfile);
	}else{
	        *str = (char*)DpsMalloc(dps_strlen(DPS_NULL2EMPTY(schema)) + dps_strlen(pathfile) + dps_strlen(DPS_NULL2EMPTY(hostinfo)) + 8);
		if (*str == NULL) return;
/*		sprintf(*str, "%s://%s%s", DPS_NULL2EMPTY(schema), DPS_NULL2EMPTY(hostinfo), pathfile);*/
		dps_strcpy(*str, DPS_NULL2EMPTY(schema)); dps_strcat(*str, "://"); dps_strcat(*str, DPS_NULL2EMPTY(hostinfo)); dps_strcat(*str, pathfile);
	}
	
	if(!strncmp(*str, "ftp://", 6) && (strstr(*str, ";type=")))
		*(strstr(*str, ";type")) = '\0';
	DPS_FREE(pathfile);

	for(cascade = 0; ((Alias=DpsMatchListFind(&Indexer->Conf->ReverseAliases,*str,nparts,Parts))) && (cascade < 1024); cascade++) {
	        aliassize = dps_strlen(Alias->arg) + dps_strlen(Alias->pattern) + dps_strlen(*str) + 128;
		alias = (char*)DpsRealloc(alias, aliassize);
		if (alias == NULL) {
		  DpsLog(Indexer, DPS_LOG_ERROR, "No memory (%d bytes). %s line %d", aliassize, __FILE__, __LINE__);
		  goto ret;
		}
		DpsMatchApply(alias,aliassize,*str,Alias->arg,Alias,nparts,Parts);
		if(alias[0]){
		  DpsLog(Indexer,DPS_LOG_DEBUG,"ReverseAlias%d: pattern:%s, arg:%s -> '%s'", cascade, Alias->pattern, Alias->arg, alias);
		  DPS_FREE(*str);
		  *str = (char*)DpsStrdup(alias);
		} else break;
	}

ret:	
	DPS_FREE(alias);

}

static int DpsDocBaseHref(DPS_AGENT *Indexer,DPS_DOCUMENT *Doc){
	const char	*basehref=DpsVarListFindStr(&Doc->Sections,"base.href",NULL);
	/* <BASE HREF="xxx"> stuff            */
	/* Check that URL is properly formed  */
	/* baseURL is just temporary variable */
	/* If parsing  fails we'll use old    */
	/* base href, passed via CurURL       */
	/* Note that we will not check BASE     */
	/* if delete_no_server is unset         */
	/* This is  actually dirty hack. We     */
	/* must check that hostname is the same */
	
	if(basehref /*&& (Doc->Spider.follow==DPS_FOLLOW_WORLD)*/){
		DPS_URL		*baseURL = DpsURLInit(NULL);
		int		parse_res;

		if (baseURL == NULL) return DPS_ERROR;

		parse_res = DpsURLParse(baseURL, basehref);
		if (!parse_res && (baseURL->schema == NULL || baseURL->hostinfo == NULL)) parse_res = DPS_URL_BAD;

		if(!parse_res) {
			DpsURLParse(&Doc->CurURL,basehref);
			DpsLog(Indexer, DPS_LOG_DEBUG, "BASE HREF '%s'", basehref);
		}else{
			switch(parse_res){
			case DPS_URL_LONG:
				DpsLog(Indexer,DPS_LOG_ERROR,"BASE HREF too long: '%s'",basehref);
				break;
			case DPS_URL_BAD:
			default:
				DpsLog(Indexer,DPS_LOG_ERROR,"Error in BASE HREF URL: '%s'",basehref);
			}
		}
		DpsURLFree(baseURL);
	}
	return DPS_OK;
}


int DpsConvertHref(DPS_AGENT *Indexer, DPS_URL *CurURL, DPS_HREF *Href){
	int		parse_res;
	DPS_URL		*newURL;
	char		*newhref = NULL;

#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif
	if ((newURL = DpsURLInit(NULL)) == NULL) {
	  return DPS_ERROR;
	}
	
	if((parse_res=DpsURLParse(newURL, Href->url))){
		switch(parse_res){
			case DPS_URL_LONG:
				DpsLog(Indexer,DPS_LOG_DEBUG,"URL too long: '%s'",Href->url);
				break;
			case DPS_URL_BAD:
			default:
				DpsLog(Indexer,DPS_LOG_DEBUG,"Error in URL: '%s'",Href->url);
		}
	}

	newURL->charset_id = Href->charset_id;
	RelLink(Indexer, CurURL, newURL, &newhref);
	
	DpsLog(Indexer,DPS_LOG_DEBUG,"Link '%s' %s",Href->url,newhref);

	DpsHrefCheck(Indexer, Href, newhref);

	DPS_FREE(Href->url);
	Href->url = (char*)DpsStrdup(newhref);
/* ret:*/
	DPS_FREE(newhref);
	DpsURLFree(newURL);
#ifdef WITH_PARANOIA
	DpsViolationExit(Indexer->handle, paran);
#endif
	return DPS_OK;
}

static int DpsDocConvertHrefs(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc){
	size_t		i;
	int		hops=DpsVarListFindInt(&Doc->Sections,"Hops",0);
	urlid_t		url_id = (urlid_t)DpsVarListFindInt(&Doc->Sections, "DP_ID", 0);
	dps_uint4           maxhops = DpsVarListFindUnsigned(&Doc->Sections, "MaxHops", 255);
	urlid_t         server_id = (urlid_t)DpsVarListFindInt(&Doc->Sections, "Server_id", 0);

	for(i=0;i<Doc->Hrefs.nhrefs;i++){
		DPS_HREF	*Href=&Doc->Hrefs.Href[i];
		Href->hops=hops+1;
		Href->charset_id = Doc->charset_id;
		DpsConvertHref(Indexer,&Doc->CurURL,Href);
		Href->referrer=url_id;
		if ((server_id != Href->server_id) || (maxhops > Href->hops)) {
		  Href->stored = 0;
		} else {
		  if (Href->method != DPS_METHOD_DISALLOW) 
		    DpsLog(Indexer, DPS_LOG_DEBUG, " link: too many hops (%d, max: %d)", Href->hops, maxhops);
		  Href->stored = 1;
		  Href->method = DPS_METHOD_DISALLOW;
		}
	}
	return DPS_OK;
}

int DpsDocStoreHrefs(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc){
	size_t i;
	float weight;

	if(Doc->method==DPS_METHOD_HEAD)
		return DPS_OK;
	
	DpsDocBaseHref(Indexer,Doc);
	DpsDocConvertHrefs(Indexer,Doc);
	weight = (Doc->Hrefs.nhrefs) ?  (1.0 / Doc->Hrefs.nhrefs) : 0.1;
	for(i=0;i<Doc->Hrefs.nhrefs;i++){
		DPS_HREF	*Href=&Doc->Hrefs.Href[i];
		if((Href->method != DPS_METHOD_DISALLOW) && (Href->method != DPS_METHOD_VISITLATER)) {
		  Href->charset_id = Doc->charset_id;
		  Href->weight = weight;
		  DpsHrefListAdd(Indexer, &Indexer->Hrefs, Href);
		}
	}
	return DPS_OK;
}

/*********************** 'UrlFile' stuff (for -f option) *******************/

__C_LINK int __DPSCALL DpsURLFile(DPS_AGENT *Indexer, const char *fname,int action){
	FILE *url_file;
	char str[1024]="";
	char str1[1024]="";
	int result, res, cnt_flag = 0;
	DPS_URL *myurl;
	DPS_HREF Href;

#ifdef WITH_PARANOIA
	void *paran = DpsViolationEnter(paran);
#endif

	if ((myurl = DpsURLInit(NULL)) == NULL) {
	  return DPS_ERROR;
	}
	
	/* Read lines and clear/insert/check URLs                     */
	/* We've already tested in main.c to make sure it can be read */
	/* FIXME !!! Checking should be done here surely              */
	
	if(!strcmp(fname,"-"))
		url_file=stdin;
	else
		url_file=fopen(fname,"r");
	
	if (action == DPS_URL_FILE_TARGET && Indexer->Conf->url_number ==  0x7FFFFFFF) {
	  cnt_flag = 1;
	  Indexer->Conf->url_number = 0;
	}

	while(fgets(str1,sizeof(str1),url_file)){
		char *end;
		if(!str1[0])continue;
		end=str1+dps_strlen(str1)-1;
		while((end>=str1)&&(*end=='\r'||*end=='\n')){
			*end=0;if(end>str1)end--;
		}
		if(!str1[0])continue;
		if(str1[0]=='#')continue;

		if(*end=='\\'){
			*end=0; dps_strcat(str, str1);
			continue;
		}
		dps_strcat(str, str1);
		dps_strcpy(str1,"");

		switch(action){
		case DPS_URL_FILE_REINDEX:
		        DpsLog(Indexer, DPS_LOG_EXTRA, "Marking for reindexing: %s", str);
		        if (strchr(str, '%'))
			  DpsVarListReplaceStr(&Indexer->Vars, "ul", str);
			else
			  DpsVarListReplaceStr(&Indexer->Vars, "ue", str);
			result = DpsURLAction(Indexer, NULL, DPS_URL_ACTION_EXPIRE);
			if(result!=DPS_OK) { DpsURLFree(myurl); 
#ifdef WITH_PARANOIA
			  DpsViolationExit(Indexer->handle, paran);
#endif
			return(result); }
			DpsVarListDel(&Indexer->Vars, "ul");
			DpsVarListDel(&Indexer->Vars, "ue");
			break;
		case DPS_URL_FILE_TARGET:
		        DpsLog(Indexer, DPS_LOG_EXTRA, "Targeting for indexing: %s", str);
		        if (strchr(str, '%'))
			  DpsVarListReplaceStr(&Indexer->Vars, "ul", str);
			else
			  DpsVarListReplaceStr(&Indexer->Vars, "ue", str);
			/*result =*/ DpsAppendTarget(Indexer, str, "", 0, 0);
/*			if(result!=DPS_OK) { DpsURLFree(myurl); 
#ifdef WITH_PARANOIA
			  DpsViolationExit(Indexer->handle, paran);
#endif
			return(result); }*/
			if (cnt_flag) Indexer->Conf->url_number++;
			DpsVarListDel(&Indexer->Vars, "ul");
			DpsVarListDel(&Indexer->Vars, "ue");
			break;
		case DPS_URL_FILE_CLEAR:
		        DpsLog(Indexer, DPS_LOG_EXTRA, "Deleting: %s", str);
		        if (strchr(str, '%'))
			  DpsVarListReplaceStr(&Indexer->Vars, "ul", str);
			else
			  DpsVarListReplaceStr(&Indexer->Vars, "ue", str);
			result=DpsClearDatabase(Indexer);
			if(result!=DPS_OK) { DpsURLFree(myurl); 
#ifdef WITH_PARANOIA
			  DpsViolationExit(Indexer->handle, paran);
#endif
			return(DPS_ERROR); }
			DpsVarListDel(&Indexer->Vars, "ul");
			DpsVarListDel(&Indexer->Vars, "ue");
			break;
		case DPS_URL_FILE_INSERT:
		        DpsLog(Indexer, DPS_LOG_EXTRA, "Inserting: %s", str);
			DpsHrefInit(&Href);
			Href.url=str;
			Href.method=DPS_METHOD_GET;
			DpsHrefListAdd(Indexer, &Indexer->Hrefs, &Href);
			if (Indexer->Hrefs.nhrefs > MAXHSIZE) DpsStoreHrefs(Indexer);
			break;
		case DPS_URL_FILE_PARSE:
		        DpsLog(Indexer, DPS_LOG_EXTRA, "Parsing: %s", str);
			res=DpsURLParse(myurl, str);
			if((res != DPS_OK) && (myurl->schema == NULL))
				res=DPS_URL_BAD;
			if(res){
				switch(res){
				case DPS_URL_LONG:
					DpsLog(Indexer,DPS_LOG_ERROR,"URL too long: '%s'",str);
					break;
				case DPS_URL_BAD:
				default:
					DpsLog(Indexer,DPS_LOG_ERROR,"Error in URL: '%s'",str);
				}
				DpsURLFree(myurl);
#ifdef WITH_PARANOIA
				DpsViolationExit(Indexer->handle, paran);
#endif
				return(DPS_ERROR);
			}
			break;
		}
		str[0]=0;
	}
	if(url_file!=stdin)
		fclose(url_file);
	DpsURLFree(myurl);
#ifdef WITH_PARANOIA
	DpsViolationExit(Indexer->handle, paran);
#endif
	return(DPS_OK);
}




/*******************************************************************/


static int DpsDocAlias(DPS_AGENT *Indexer,DPS_DOCUMENT *Doc){
	DPS_MATCH	*Alias;
	DPS_MATCH_PART	Parts[10];
	size_t		alstrlen, nparts=10;
	const char	*alias_prog=DpsVarListFindStr(&Indexer->Vars, "AliasProg", NULL);
	char		*alstr;
	int		result=DPS_OK;
	const char	*url=DpsVarListFindStr(&Doc->Sections,"URL","");

	alstrlen = 256 + dps_strlen(url);
	if ((alstr = (char*)DpsMalloc(alstrlen)) == NULL) return DPS_ERROR;
	alstr[0] = '\0';
	if(alias_prog){
		result = DpsAliasProg(Indexer, alias_prog, url, alstr, alstrlen - 1);
		DpsLog(Indexer,DPS_LOG_EXTRA,"AliasProg result: '%s'",alstr);
		if(result!=DPS_OK) { DPS_FREE(alstr); return result; }
	}
	
	/* Find alias when aliastr is empty, i.e.     */
	/* when there is no alias in "Server" command */
	/* and no AliasProg                           */
	if( !alstr[0] && (Alias = DpsMatchListFind(&Indexer->Conf->Aliases, url, nparts, Parts))) {
		DpsMatchApply(alstr, alstrlen - 1, url, Alias->arg, Alias, nparts, Parts);
	}
	if(alstr[0]){
		DpsVarListReplaceStr(&Doc->Sections,"Alias",alstr);
	}
	DPS_FREE(alstr);
	return DPS_OK;
}




static int DpsDocCheck(DPS_AGENT *Indexer, DPS_SERVER *CurSrv, DPS_DOCUMENT *Doc) {
	char		reason[1024]="";
	int		nerrors=-1;
	int		hops=DpsVarListFindInt(&Doc->Sections,"Hops",0);
	const char	*method=DpsVarListFindStr(&CurSrv->Vars,"Method","Allow");
	const int       older = DpsVarListFindInt(&Doc->Sections, "DeleteOlder", 0);
	float           site_weight;
	size_t          depth;
	const char      *s;
	
	TRACE_IN(Indexer, "DpsDocCheck");

	switch(CurSrv->Match.match_type){
		case DPS_MATCH_WILD:
			DpsLog(Indexer,DPS_LOG_DEBUG, "Realm %s wild '%s'", method, CurSrv->Match.pattern);
			break;
		case DPS_MATCH_REGEX:
			DpsLog(Indexer,DPS_LOG_DEBUG, "Realm %s regex '%s'", method, CurSrv->Match.pattern);
			break;
		case DPS_MATCH_SUBNET:
			DpsLog(Indexer,DPS_LOG_DEBUG, "Subnet %s '%s'", method, CurSrv->Match.pattern);
			break;
		case DPS_MATCH_BEGIN:
		default:
			DpsLog(Indexer,DPS_LOG_DEBUG, "Server %s '%s'", method, CurSrv->Match.pattern);
			break;
	}
	
	if((Doc->method = DpsMethod(method)) != DPS_METHOD_DISALLOW) {  /* was: == DPS_METHOD_GET */
	  /* Check Allow/Disallow/CheckOnly stuff */
	  Doc->method=DpsFilterFind(DPS_LOG_DEBUG, &Indexer->Conf->Filters, DpsVarListFindStr(&Doc->Sections,"URL",""), reason,Doc->method);
	  DpsLog(Indexer, DPS_LOG_DEBUG,"%s", reason);
	}
	
	if(Doc->method==DPS_METHOD_DISALLOW) {
	  TRACE_OUT(Indexer);
	  return DPS_OK;
	}
	
	/* Check that hops is less than MaxHops */
	if(hops>Doc->Spider.maxhops){
	        DpsLog(Indexer, DPS_LOG_WARN, "Too many hops (%d, max: %d)", hops, Doc->Spider.maxhops);
		Doc->method = DPS_METHOD_DISALLOW;
		TRACE_OUT(Indexer);
		return DPS_OK;
	}

	/* Check that depth is less than MaxDepth */
	depth = 0;
	for(s = strchr(Doc->CurURL.path, (int)'/'); s != NULL; s = strchr(++s, (int)'/')) depth++;
	if (depth > CurSrv->MaxDepth) {
	  DpsLog(Indexer, DPS_LOG_DEBUG, "too deep depth (%d, max: %d), skip it", depth, CurSrv->MaxDepth);
	  Doc->method = DPS_METHOD_DISALLOW;
	  TRACE_OUT(Indexer);
	  return DPS_OK;
	}

	/* Check for older focs */
	if (older > 0) {
	  time_t now = Indexer->now, last_mod_time = DpsHttpDate2Time_t(DpsVarListFindStr(&Doc->Sections, "Last-Modified", ""));

	  if (last_mod_time > 0) {
	    if ((int)(now - last_mod_time) > older) {
	      DpsLog(Indexer, DPS_LOG_WARN, "Too old document (%d > %d)", now - last_mod_time, older);
	      Doc->method = DPS_METHOD_DISALLOW;
	      TRACE_OUT(Indexer);
	      return DPS_OK;
	    }
	  } else {
	    time_t since = (time_t)DpsVarListFindInt(&Doc->Sections, "Since", 0);
	    if ((int)(now - since) > older) {
	      DpsLog(Indexer, DPS_LOG_WARN, "Too old document (%d > %d)", now - since, older);
	      Doc->method = DPS_METHOD_DISALLOW;
	      TRACE_OUT(Indexer);
	      return DPS_OK;
	    }
	  }
	}

	/* Check for too many errors on this server */
	DpsDocLookupConn(Indexer, Doc);
	nerrors = (Doc->connp.Host != NULL) ?  Doc->connp.Host->net_errors : 0;
	
	if((nerrors>=Doc->Spider.max_net_errors)&&(Doc->Spider.max_net_errors)){
		time_t	next_index_time = Indexer->now + Doc->Spider.net_error_delay_time;
		char	buf[64];
		
		DpsLog(Indexer,DPS_LOG_WARN,"Too many network errors (%d) for this server",nerrors);
		DpsVarListReplaceInt(&Doc->Sections,"Status",DPS_HTTP_STATUS_SERVICE_UNAVAILABLE);
		dps_snprintf(buf, sizeof(buf), "%lu", (next_index_time & 0x80000000) ? 0x7fffffff : next_index_time);
		DpsVarListReplaceStr(&Doc->Sections,"Next-Index-Time",buf);
		Doc->method = DPS_METHOD_VISITLATER;
		TRACE_OUT(Indexer);
		return DPS_OK;
	}

	/* Check referring status, if need */
	if (Indexer->Flags.skip_unreferred && ((Indexer->flags & DPS_FLAG_REINDEX) == 0) && (DpsCheckReferrer(Indexer, Doc) != DPS_OK)) {
	  int prevstatus = DpsVarListFindInt(&Doc->Sections, "PrevStatus", 0);
	  if (prevstatus > 0) {
	    DpsLog(Indexer, DPS_LOG_EXTRA, "Unreferred, %s it",(Indexer->Flags.skip_unreferred == DPS_METHOD_DISALLOW) ? "delete" : "skip");
	    Doc->method = Indexer->Flags.skip_unreferred; /* DPS_METHOD_VISITLATER or DPS_METHO_DISALLOW */
	    if (prevstatus < 400) {
	      DpsVarListReplaceInt(&Doc->Sections,"Status", DPS_HTTP_STATUS_NOT_MODIFIED);
	    } else {
	      DpsVarListReplaceInt(&Doc->Sections, "Status", prevstatus);
	    }
	    TRACE_OUT(Indexer);
	    return DPS_OK;
	  }
	}

	DpsVarListReplaceInt(&Doc->Sections, "Site_id", DpsServerGetSiteId(Indexer, CurSrv, Doc));
	site_weight = (float)DpsVarListFindDouble(&Doc->Sections, "SiteWeight", 0.0);
	/* Check weights */
	if (site_weight < CurSrv->MinSiteWeight) {
	  DpsLog(Indexer, DPS_LOG_EXTRA, "Too low site weight (%f < %f)", site_weight, CurSrv->MinSiteWeight);
	      Doc->method = DPS_METHOD_VISITLATER;
	      TRACE_OUT(Indexer);
	      return DPS_OK;
	}
	if (CurSrv->weight < CurSrv->MinServerWeight) {
	  DpsLog(Indexer, DPS_LOG_EXTRA, "Too low server weight (%f < %f)", CurSrv->weight, CurSrv->MinServerWeight);
	      Doc->method = DPS_METHOD_VISITLATER;
	      TRACE_OUT(Indexer);
	      return DPS_OK;
	}
	

	{
	  const char *param = DpsVarListFindStr(&CurSrv->Vars, "IndexDocSizeLimit", NULL);

	  if (param != NULL) DpsVarListAddStr(&Doc->Sections, "IndexDocSizeLimit", param);
	}
	
	TRACE_OUT(Indexer);
	return DPS_OK;
}


static int DpsParseSections(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc) {
  DPS_MATCH       *Alias;
  DPS_MATCH_PART  Parts[10];
  size_t nparts = 10;
  DPS_VAR *Sec;
  DPS_VAR S;
  char *lt, *buf;
  const char *buf_content = (Doc->Buf.pattern == NULL) ? Doc->Buf.content : Doc->Buf.pattern;
  DPS_TEXTITEM Item;
  DPS_HREF     Href;
  size_t i, buf_len;

  if (Indexer->Conf->SectionMatch.nmatches == 0 && Indexer->Conf->HrefSectionMatch.nmatches == 0) return DPS_OK;

  buf = (char*)DpsMalloc(buf_len = (Doc->Buf.size + 1024));
  if (buf == NULL) return DPS_OK;

  for (i = 0; i < Indexer->Conf->SectionMatch.nmatches; i++) {
    Alias = &Indexer->Conf->SectionMatch.Match[i];

/*    fprintf(stderr, " -- Alias:%s, before match\n", Alias->section);*/
    if (DpsMatchExec(Alias, buf_content, buf_content, NULL, nparts, Parts)) continue;

    Sec = DpsVarListFind(&Doc->Sections, Alias->section);
/*    fprintf(stderr, " -- Alias:%s, Sec: %x\n", Alias->section, Sec);*/
    if (! Sec) continue;

    DpsMatchApply(buf, buf_len - 1, buf_content, Alias->arg, Alias, nparts, Parts);
/*    fprintf(stderr, " -- Alias:%s, after match apply: %s\n", Alias->section, buf);*/

    if (Sec->maxlen) {
      bzero((void*)&S, sizeof(S));
      S.name = Sec->name;
      S.section = Sec->section;
      S.strict = Sec->strict;
      S.maxlen = Sec->maxlen;
      S.val = buf;
      DpsVarListAdd(&Doc->Sections, &S);
    }

    Item.href = NULL;
    Item.section = Sec->section;
    Item.strict = Sec->strict;
    Item.section_name = Sec->name;
    Item.str = dps_strtok_r(buf, "\r\n", &lt);
    while(Item.str) {
      Item.len = (lt != NULL) ? (lt - Item.str) : 0 /*dps_strlen(Item.str)*/;
/*      fprintf(stderr, " -- Alias:%s, [%d] %s\n", Alias->section, Item.len, Item.str);*/
      DpsTextListAdd(&Doc->TextList, &Item);
      Item.str = dps_strtok_r(NULL, "\r\n", &lt);
    }
  }

  for (i = 0; i < Indexer->Conf->HrefSectionMatch.nmatches; i++) {
    Alias = &Indexer->Conf->HrefSectionMatch.Match[i];

    if (DpsMatchExec(Alias, buf_content, buf_content, NULL, nparts, Parts)) continue;

    Sec = DpsVarListFind(&Indexer->Conf->HrefSections, Alias->section);
    if (! Sec) continue;

    DpsMatchApply(buf, buf_len - 1, buf_content, Alias->arg, Alias, nparts, Parts);

    DpsHrefInit(&Href);
    Href.referrer = DpsVarListFindInt(&Doc->Sections, "Referrer-ID", 0);
    Href.hops = 1 + DpsVarListFindInt(&Doc->Sections,"Hops",0);
    Href.site_id = 0; /*DpsVarListFindInt(&Doc->Sections, "Site_id", 0);*/
    Href.url = buf;
    Href.method = DPS_METHOD_GET;
    DpsHrefListAdd(Indexer, &Doc->Hrefs, &Href);
  }

  DPS_FREE(buf);
  return DPS_OK;
}


static int DpsExecActions(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc) {
  DPS_MATCH       *Alias;
  DPS_MATCH_PART  Parts[10];
  size_t nparts = 10;
  DPS_VAR *Sec;
  char *buf;
  DPS_TEXTITEM *Item;
  size_t i, z, buf_len;

  if (Indexer->Conf->ActionSQLMatch.nmatches == 0) return DPS_OK;

  buf = (char*)DpsMalloc(buf_len = (Doc->Buf.size + 1024));
  if (buf == NULL) return DPS_OK;

  {
    char qbuf[16384], cbuf[16384];
    DPS_TEMPLATE t;
    DPS_DBLIST      dbl;
    DPS_DB *db;
    const char	*doccset;
    DPS_CHARSET	*doccs, *loccs;
    DPS_CONV	dc_lc;
    bzero(&t, sizeof(t));
    t.HlBeg = t.HlEnd = t.GrBeg = t.GrEnd = NULL;
    t.Env_Vars = &Doc->Sections;

    doccset=DpsVarListFindStr(&Doc->Sections,"Charset",NULL);
    if(!doccset||!*doccset)doccset=DpsVarListFindStr(&Doc->Sections,"RemoteCharset","iso-8859-1");
    doccs=DpsGetCharSet(doccset);
    if(!doccs)doccs=DpsGetCharSet("iso-8859-1");
    loccs = Doc->lcs;
    if (!loccs) loccs = Indexer->Conf->lcs;
    if (!loccs) loccs = DpsGetCharSet("iso-8859-1");
    DpsConvInit(&dc_lc, doccs, loccs, Indexer->Conf->CharsToEscape, DPS_RECODE_HTML);

    for (i = 0; i < Indexer->Conf->ActionSQLMatch.nmatches; i++) {
      DPS_TEXTLIST	*tlist = &Doc->TextList;
      Alias = &Indexer->Conf->ActionSQLMatch.Match[i];

      Sec = DpsVarListFind(&Indexer->Conf->Sections, Alias->section);
      if (! Sec) continue;
      if (Alias->dbaddr != NULL) {
	DpsDBListInit(&dbl);
	DpsDBListAdd(&dbl, Alias->dbaddr, DPS_OPEN_MODE_READ);
	db = &dbl.db[0];
      } else {
	db = (Indexer->flags & DPS_FLAG_UNOCON) ? &Indexer->Conf->dbl.db[0] :  &Indexer->dbl.db[0];
      }
      for(z = 0; z < tlist->nitems; z++) {
	Item = &tlist->Items[z];
	if (Item->section != Sec->section) continue;
	if (strcasecmp(Item->section_name, Alias->section)) continue;
	if (DpsMatchExec(Alias, Item->str, Item->str, NULL, nparts, Parts)) continue;
/*	fprintf(stderr, " -- SQLAction:%s, before match apply: %s\n", Alias->section, Alias->arg);*/
	DpsMatchApply(buf, buf_len - 1, Item->str, Alias->arg, Alias, nparts, Parts);
/*	fprintf(stderr, " -- SQLAction:%s, after match apply: %s\n", Alias->section, buf);*/
	DpsConv(&dc_lc, cbuf, sizeof(cbuf), buf, buf_len);
/*	fprintf(stderr, " -- SQLAction:%s, after dc_lc conv: %s\n", Alias->section, cbuf);*/
	DpsPrintTextTemplate(Indexer, NULL, NULL, qbuf, sizeof(qbuf), &t, cbuf);
/*	fprintf(stderr, " -- SQLAction:%s, after template patch: %s\n", Alias->section, qbuf);*/
	if (DPS_OK != DpsSQLAsyncQuery(db, NULL, qbuf)) DpsLog(Indexer, DPS_ERROR, "ActionSQL error");
      }
      if (Alias->dbaddr != NULL) DpsDBListFree(&dbl);
    }  
    DpsTemplateFree(&t);
  }

  DPS_FREE(buf);
  return DPS_OK;
}

static int DpsDocParseContent(DPS_AGENT * Indexer, DPS_DOCUMENT * Doc) {
	
#ifdef WITH_PARSER
	DPS_PARSER	*Parser;
#endif
	const int       status = DpsVarListFindInt(&Doc->Sections, "Status", 0);
	const char	*real_content_type=NULL;
	const char	*url=DpsVarListFindStr(&Doc->Sections,"URL","");
	const char	*ct=DpsVarListFindStr(&Doc->Sections,"Content-Type","");
	const char	*ce=DpsVarListFindStr(&Doc->Sections,"Content-Encoding","");
	int		result=DPS_OK;
	
	if(!strcmp(DPS_NULL2EMPTY(Doc->CurURL.filename), "robots.txt")) return DPS_OK;

	if(Doc->method != DPS_METHOD_HEAD) {
	
#ifdef HAVE_ZLIB
	  if((!strcasecmp(ce, "gzip")) || (!strcasecmp(ce, "x-gzip"))) {
		DPS_THREADINFO(Indexer,"UnGzip",url);
		if (status == 206) {
		  DpsLog(Indexer, DPS_LOG_INFO, "Parial content, can't ungzip it.");
		  return result;
		}
		DpsUnGzip(Indexer, Doc);
		DpsVarListReplaceInt(&Doc->Sections, "Content-Length", Doc->Buf.buf - Doc->Buf.content + (int)Doc->Buf.size);
	  }else
	  if(!strcasecmp(ce,"deflate")){
		DPS_THREADINFO(Indexer,"Inflate",url);
		if (status == 206) {
		  DpsLog(Indexer, DPS_LOG_INFO, "Parial content, can't inflate it.");
		  return result;
		}
		DpsInflate(Indexer, Doc);
		DpsVarListReplaceInt(&Doc->Sections, "Content-Length", Doc->Buf.buf - Doc->Buf.content + (int)Doc->Buf.size);
	  }else
	    if((!strcasecmp(ce, "compress")) || (!strcasecmp(ce, "x-compress"))) {
		DPS_THREADINFO(Indexer,"Uncompress",url);
		if (status == 206) {
		  DpsLog(Indexer, DPS_LOG_INFO, "Parial content, can't uncomress it.");
		  return result;
		}
		DpsUncompress(Indexer, Doc);
		DpsVarListReplaceInt(&Doc->Sections, "Content-Length", Doc->Buf.buf - Doc->Buf.content + (int)Doc->Buf.size);
	  }else
#endif	
	      if((!strcasecmp(ce,"identity")) || (!strcasecmp(ce,""))) {
		/* Nothing to do*/
	  }else{
		DpsLog(Indexer,DPS_LOG_ERROR,"Unsupported Content-Encoding");
/*		DpsVarListReplaceInt(&Doc->Sections,"Status",DPS_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);*/
	  }
	
#ifdef WITH_PARSER
	  /* Let's try to start external parser for this Content-Type */

	  if((Parser = DpsParserFind(&Indexer->Conf->Parsers, ct))) {
		DpsLog(Indexer,DPS_LOG_DEBUG,"Found external parser '%s' -> '%s'",
			Parser->from_mime?Parser->from_mime:"NULL",
			Parser->to_mime?Parser->to_mime:"NULL");
	  }
	  if(Parser) {
	    if (status == DPS_HTTP_STATUS_OK) {
	      if (DpsParserExec(Indexer, Parser, Doc)) {
		char *to_charset;
		real_content_type=Parser->to_mime?Parser->to_mime:"unknown";
		DpsLog(Indexer,DPS_LOG_DEBUG,"Parser-Content-Type: %s",real_content_type);
		if((to_charset=strstr(real_content_type,"charset="))){
		        const char *cs = DpsCharsetCanonicalName(DpsTrim(to_charset + 8, " \t;\"'"));
			DpsVarListReplaceStr(&Doc->Sections, "Server-Charset", cs);
			DpsLog(Indexer,DPS_LOG_DEBUG, "to_charset='%s'", cs);
		}
#ifdef DEBUG_PARSER
		fprintf(stderr,"content='%s'\n",Doc->content);
#endif
	      }
	    } else {
	      DpsLog(Indexer, DPS_LOG_WARN, "Parser is not executed, document status: %d", status);
	      return result;
	    }
	  }
#endif
	
	  if(!real_content_type)real_content_type=ct;
	  DpsVarListAddStr(&Doc->Sections,"Parser-Content-Type",real_content_type);

	  /* CRC32 without headers */
	  {
	    size_t crclen=Doc->Buf.size - (Doc->Buf.content-Doc->Buf.buf);
	    DpsVarListReplaceInt(&Doc->Sections, "crc32", (int)DpsHash32(Doc->Buf.content, crclen));
	  }

	  if (Indexer->Conf->BodyPatterns.nmatches != 0) {
	    char            *buf;
	    DPS_MATCH       *Alias;
	    DPS_MATCH_PART  Parts[10];
	    size_t nparts = 10;
	    size_t i, buf_len;

	    buf = (char*)DpsMalloc(buf_len = (Doc->Buf.size + 1024));
	    if (buf == NULL) return DPS_OK;

	    for (i = 0; i < Indexer->Conf->BodyPatterns.nmatches; i++) {
	      Alias = &Indexer->Conf->BodyPatterns.Match[i];
	      
	      if (DpsMatchExec(Alias, Doc->Buf.content, Doc->Buf.content, NULL, nparts, Parts)) continue;
	      DpsMatchApply(buf, buf_len - 1, Doc->Buf.content, Alias->arg, Alias, nparts, Parts);
	      DPS_FREE(Doc->Buf.pattern);
	      Doc->Buf.pattern = DpsRealloc(buf, dps_strlen(buf) + 1);
	      DpsLog(Indexer, DPS_LOG_DEBUG, "%dth BodyPattern applied.", i);
	      break;
	    }
/*	    dps_memmove(Doc->Buf.content, buf, buf_len = dps_strlen(buf) + 1);
	    Doc->Buf.size = buf_len + (Doc->Buf.content - Doc->Buf.buf);*/
	    if (Doc->Buf.pattern == NULL) DPS_FREE(buf);

	  }

#ifdef WITH_MP3
	  if(Doc->method == DPS_METHOD_CHECKMP3ONLY ||
	     !strncasecmp(real_content_type, "audio/mpeg",10) || !strncasecmp(real_content_type, "audio/x-mpeg",12)) {
			DpsMP3Parse(Indexer,Doc);
	  } else
#endif
	  if(!strncasecmp(real_content_type,"text/plain",10)){
			DpsParseText(Indexer, Doc);
			DpsParseSections(Indexer, Doc);
	  }else
	  if(!strncasecmp(real_content_type,"text/tab-separated-values",25)){
			DpsParseText(Indexer, Doc);
			DpsParseSections(Indexer, Doc);
	  }else
	  if(!strncasecmp(real_content_type,"text/css",8)){
			DpsParseText(Indexer, Doc);
			DpsParseSections(Indexer, Doc);
	  }else	
	  if(!strncasecmp(real_content_type,"text/html",9)
	     || !strncasecmp(real_content_type, "application/xhtml+xml", 21)
	     || !strncasecmp(real_content_type, "text/vnd.wap.wml", 16)
	     ){
			DpsHTMLParse(Indexer,Doc);
			DpsParseSections(Indexer, Doc);
	  }else
	  if(!strncasecmp(real_content_type, "text/xml", 8) 
	     || !strncasecmp(real_content_type, "application/xml", 15)
	     || !strncasecmp(real_content_type, "application/x.xml", 17)
	     || !strncasecmp(real_content_type, "application/atom+xml", 20)
	     || !strncasecmp(real_content_type, "application/x.atom+xml", 22)
	     || !strncasecmp(real_content_type, "application/x.rss+xml", 21)
	     || !strncasecmp(real_content_type, "application/rss+xml", 19) ) {
			DpsXMLParse(Indexer, Doc);
			DpsParseSections(Indexer, Doc);
	  }else
	  if(!strncasecmp(real_content_type, "image/gif", 9)) {
			DpsGIFParse(Indexer, Doc);
	  }else {
			/* Unknown Content-Type  */
			DpsLog(Indexer,DPS_LOG_ERROR,"Unsupported Content-Type '%s'",real_content_type);
			DpsVarListReplaceInt(&Doc->Sections,"Status",DPS_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE);
	  }
	}
	/* Guesser stuff */	
	if(Indexer->Flags.nmaps && Doc->method != DPS_METHOD_DISALLOW) {
	  register size_t t;
	  int flag = Indexer->Flags.update_lm;

	  bzero((void*)Indexer->LangMap, sizeof(DPS_LANGMAP));
	  for (t = 0; t <= DPS_LM_HASHMASK; t++) Indexer->LangMap->memb3[t].index=Indexer->LangMap->memb6[t].index=t;
	  for(t = 0; t < Doc->TextList.nitems; t++) {
	    DPS_TEXTITEM *Item = &Doc->TextList.Items[t];
	    DpsBuildLangMap(Indexer->LangMap, Item->str, dps_strlen(Item->str), Indexer->Flags.GuesserBytes, flag);
	    if (Indexer->Flags.GuesserBytes > 0 && Indexer->LangMap->nbytes >= Indexer->Flags.GuesserBytes) break;
	  }
	}
			
	DpsGuessCharSet(Indexer, Doc, &Indexer->Conf->LangMaps, Indexer->LangMap);
				
	DpsLog(Indexer, DPS_LOG_EXTRA, "Guesser bytes: %d, Lang: %s, Charset: %s", Indexer->Flags.GuesserBytes,
	       DpsVarListFindStr(&Doc->Sections,"Content-Language",""),
	       DpsVarListFindStr(&Doc->Sections,"Charset",""));
	/* /Guesser stuff */	


#ifdef HAVE_ZLIB
	if ((Doc->method != DPS_METHOD_HEAD) && (strncmp(real_content_type, "text/", 5) == 0)) {
	  char reason[PATH_MAX+1];
	  int m =  DpsStoreFilterFind(DPS_LOG_DEBUG, &Indexer->Conf->StoreFilters, Doc, reason);
	  DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
	  if (m == DPS_METHOD_STORE && !(Indexer->flags & DPS_FLAG_FROM_STORED)) DpsStoreDoc(Indexer, Doc, DpsVarListFindStr(&Doc->Sections, "ORIG_URL", NULL));
	}
	
#endif
	return result;
}



static int DpsNextTarget(DPS_AGENT * Indexer, DPS_DOCUMENT *Result) {
	int	result=DPS_NOTARGET;
	int     u;

	Indexer->now = time(NULL);
	DPS_GETLOCK(Indexer, DPS_LOCK_THREAD);
	u = (Indexer->Conf->url_number <= 0);

	if (u) {
	        DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
		return DPS_NOTARGET;
	}
	
	/* Load targets into memory cache */
	u = (Indexer->Conf->Targets.cur_row >= Indexer->Conf->Targets.num_rows);
	if (u) {
	        result = (Indexer->action == DPS_OK) ? DpsTargets(Indexer) : Indexer->action;
		if(result!=DPS_OK) {
		  DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
		  return result;
		}
	}
	
	/* Choose next target */
	if(Indexer->Conf->Targets.num_rows && (Indexer->Conf->Targets.cur_row < Indexer->Conf->Targets.num_rows ) ) {
		DPS_DOCUMENT *Doc=&Indexer->Conf->Targets.Doc[Indexer->Conf->Targets.cur_row];

		if (Doc != NULL) {
		  DpsVarListReplaceLst(&Result->Sections, &Indexer->Conf->Sections, NULL, "*");
		  DpsVarListReplaceLst(&Result->Sections, &Doc->Sections, NULL, "*");
		  DpsVarListReplaceLst(&Result->RequestHeaders, &Doc->RequestHeaders, NULL, "*");
		  Result->charset_id = Doc->charset_id;
		  
		  Indexer->Conf->Targets.cur_row++;
		  Indexer->Conf->url_number--;
		  DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
		  DpsVarListLog(Indexer, &Result->Sections, DPS_LOG_DEBUG, "Target");
		  return DPS_OK;
		}
	}
	
	Indexer->Conf->url_number = -1;
	DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
	return DPS_NOTARGET;
}

int DpsVarList2Doc(DPS_DOCUMENT *Doc, DPS_SERVER *Server) {
  DPS_SPIDERPARAM *S = &Doc->Spider;
  DPS_VARLIST *V = &Server->Vars;
/*  char str[64];
  size_t i;
  time_t default_period = (time_t)DpsVarListFindUnsigned(V, "Period", DPS_DEFAULT_REINDEX_TIME);*/

	S->maxhops		= DpsVarListFindInt(V, "MaxHops",	DPS_DEFAULT_MAX_HOPS);
	S->follow		= DpsVarListFindInt(V, "Follow",		DPS_FOLLOW_PATH);
	S->max_net_errors	= DpsVarListFindInt(V, "MaxNetErrors",	DPS_MAXNETERRORS);
	S->net_error_delay_time	= DpsVarListFindInt(V, "NetErrorDelayTime",DPS_DEFAULT_NET_ERROR_DELAY_TIME);
	Doc->connp.timeout = S->read_timeout = DpsVarListFindInt(V, "ReadTimeOut",	DPS_READ_TIMEOUT);
	S->doc_timeout		= DpsVarListFindInt(V, "DocTimeOut",	DPS_DOC_TIMEOUT);
	S->index		= DpsVarListFindInt(V, "Index",		1);
	S->use_robots		= Server->use_robots;
	S->use_clones		= DpsVarListFindInt(V, "DetectClones",	1);
	S->use_cookies		= DpsVarListFindInt(V, "Cookies",	0);
	S->Server               = Server;
/*
	for (i = 0; i < DPS_DEFAULT_MAX_HOPS; i++) {
	  dps_snprintf(str, sizeof(str), "Period%u", i);
	  S->period[i] = (time_t)DpsVarListFindUnsigned(V, str, default_period);
	}
*/
	return DPS_OK;
}

__C_LINK int __DPSCALL DpsIndexSubDoc(DPS_AGENT *Indexer, DPS_DOCUMENT *Parent, const char *base, const char *lang, const char *url) {
	DPS_DOCUMENT	*Doc;
	DPS_URL		*newURL, *baseURL = NULL;
	const char	*alias = NULL;
	char		*newhref = NULL;
	char		*origurl = NULL, *aliasurl = NULL;
	DPS_SERVER	*Server = NULL;
	int		result = DPS_OK, status = 0, parse_res;
	size_t          i;
#ifdef WITH_PARANOIA
	void * paran = DpsViolationEnter(paran);
#endif

	TRACE_IN(Indexer, "DpsIndexSubDoc");
	DPS_THREADINFO(Indexer,"Starting subdoc","");
	
	if ((Doc = DpsDocInit(NULL)) == NULL) {
	  TRACE_OUT(Indexer);
	  return DPS_ERROR;
	}
	Doc->subdoc = Parent->subdoc + 1;
	Doc->Spider = Parent->Spider;
	
	if ((newURL = DpsURLInit(NULL)) == NULL) {
	  DpsDocFree(Doc);
	  TRACE_OUT(Indexer);
	  return DPS_ERROR;
	}

	if((parse_res=DpsURLParse(newURL, url))) {
		Doc->method = DPS_METHOD_DISALLOW;
		switch(parse_res){
			case DPS_URL_LONG:
				DpsLog(Indexer, DPS_LOG_DEBUG, "URL too long: '%s'", url);
				break;
			case DPS_URL_BAD:
			default:
				DpsLog(Indexer, DPS_LOG_DEBUG, "Error in URL: '%s'", url);
		}
	}
	if (base != NULL) {
	  if ((baseURL = DpsURLInit(NULL)) == NULL) {
	    DpsURLFree(newURL);
	    DpsDocFree(Doc);
	    TRACE_OUT(Indexer);
	    return DPS_ERROR;
	  }
	  if((parse_res = DpsURLParse(baseURL, base))) {
		Doc->method = DPS_METHOD_DISALLOW;
		switch(parse_res){
			case DPS_URL_LONG:
				DpsLog(Indexer, DPS_LOG_DEBUG, "Base URL too long: '%s'", base);
				break;
			case DPS_URL_BAD:
			default:
				DpsLog(Indexer, DPS_LOG_DEBUG, "Error in base URL: '%s'", base);
		}
	  }
	}

	newURL->charset_id = Parent->charset_id;
	RelLink(Indexer, (base) ? baseURL : &Parent->CurURL, newURL, &newhref);
	DpsVarListReplaceLst(&Doc->Sections, &Parent->Sections, NULL, "*");
	DpsVarListDel(&Doc->Sections, "E_URL");
	DpsVarListDel(&Doc->Sections, "URL_ID");
	DpsVarListDel(&Doc->Sections, "DP_ID");
	DpsVarListDel(&Doc->Sections, "Content-Language");
/*	DpsVarListDel(&Doc->Sections, "base.href");*/

	DpsVarListReplaceStr(&Doc->Sections, "URL", newhref);
	DpsVarListReplaceStr(&Doc->Sections, "base.href", (base) ? base : DpsVarListFindStr(&Parent->Sections, "URL", url));
	DpsVarListReplaceInt(&Doc->Sections, "crc32old", DpsVarListFindInt(&Doc->Sections, "crc32", 0));
	if (lang != NULL) {
	  Doc->fetched = 1;
	  DpsVarListReplaceStr(&Doc->RequestHeaders, "Accept-Language", lang);
	  DpsVarListReplaceStr(&Doc->Sections, "Content-Language", lang);
	}
	DpsLog(Indexer, (Indexer->Flags.cmd != DPS_IND_POPRANK) ? DPS_LOG_INFO : DPS_LOG_DEBUG, "[%s] Subdoc level:%d, URL: %s", 
	       DPS_NULL2EMPTY(lang), Doc->subdoc, newhref);
	
	/* To see the URL being indexed in "ps" output on xBSD */
	if (Indexer->Flags.cmd != DPS_IND_POPRANK) dps_setproctitle("[%d] Subdoc:%s", Indexer->handle, newhref);
	
	/* Collect information from Conf */
	
	if(Indexer->Flags.cmd != DPS_IND_POPRANK && !Doc->Buf.buf) {
		/* Alloc buffer for document */
		Doc->Buf.max_size = (size_t)DpsVarListFindInt(&Indexer->Vars, "MaxDocSize", DPS_MAXDOCSIZE);
		Doc->Buf.allocated_size = DPS_NET_BUF_SIZE;
		if ((Doc->Buf.buf = (char*)DpsRealloc(Doc->Buf.buf, Doc->Buf.allocated_size + 1)) == NULL) {
		  DpsLog(Indexer, DPS_LOG_ERROR, "Out of memory (%d bytes) %s:%d", Doc->Buf.allocated_size + 1, __FILE__, __LINE__);
		  DpsDocFree(Doc);
		  if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
		  TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		  DpsViolationExit(Indexer->handle, paran);
#endif
		  return DPS_ERROR;
		}
		Doc->Buf.buf[0]='\0';
	}
	
	/* Check that URL has valid syntax */
	if(DpsURLParse(&Doc->CurURL, newhref)) {
		DpsLog(Indexer,DPS_LOG_WARN,"Invalid URL: %s", newhref);
		Doc->method = DPS_METHOD_DISALLOW;
	}else
	if ((Doc->CurURL.filename != NULL) && (!strcmp(Doc->CurURL.filename, "robots.txt"))) {
		Doc->method = DPS_METHOD_DISALLOW;
	}else if (Indexer->Flags.cmd != DPS_IND_POPRANK) {
		char		*alstr = NULL;

		Doc->CurURL.charset_id = Doc->charset_id;
		/* Find correspondent Server */
/*		DPS_GETLOCK(Indexer,DPS_LOCK_CONF);*/
		Server = DpsServerFind(Indexer, (urlid_t)DpsVarListFindInt(&Doc->Sections, "Server_id", 0), 
				       newhref, Doc->charset_id, &alstr);
/*		DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);*/
		if ( !Server ) Server = Parent->Server;
		if ( !Server ) {
			DpsLog(Indexer,DPS_LOG_WARN,"No 'Server' command for url");
			Doc->method = DPS_METHOD_DISALLOW;
		} else {
		        DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
			Doc->lcs = Indexer->Conf->lcs;
			DpsVarList2Doc(Doc, Server);
			Doc->Spider.ExpireAt = Server->ExpireAt;
			Doc->Server = Server;
			
			DpsDocAddConfExtraHeaders(Indexer->Conf, Doc);
			DpsDocAddServExtraHeaders(Server, Doc);
			DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);

			DpsVarListReplaceLst(&Doc->Sections, &Server->Vars,NULL,"*");
/*			DpsVarListReplaceInt(&Doc->Sections, "Site_id", DpsServerGetSiteId(Indexer, Server, Doc));*/
			DpsVarListReplaceInt(&Doc->Sections, "Server_id", Server->site_id);
			DpsVarListReplaceInt(&Doc->Sections, "MaxHops", Doc->Spider.maxhops);
			
			if(alstr != NULL) {
				/* Server Primary alias found */
				DpsVarListReplaceStr(&Doc->Sections, "Alias", alstr);
			}else{
				/* Apply non-primary alias */
				result = DpsDocAlias(Indexer, Doc);
			}

			if((alias = DpsVarListFindStr(&Doc->Sections, "Alias", NULL))) {
			  const char *u = DpsVarListFindStr(&Doc->Sections, "URL", NULL);
			  origurl = (char*)DpsStrdup(u);
			  aliasurl = (char*)DpsStrdup(alias);
			  DpsLog(Indexer,DPS_LOG_EXTRA,"Alias: '%s'", alias);
			  DpsVarListReplaceStr(&Doc->Sections, "URL", alias);
			  DpsVarListReplaceStr(&Doc->Sections, "ORIG_URL", origurl);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL, alias);
			}

			/* Check hops, network errors, filters */
			result = DpsDocCheck(Indexer, Server, Doc);
			switch(Doc->method) {
			case DPS_METHOD_GET:
			case DPS_METHOD_HEAD:
			case DPS_METHOD_HREFONLY:
			case DPS_METHOD_CHECKMP3:
			case DPS_METHOD_CHECKMP3ONLY:
			  if( (Server->MaxDocsPerServer != (dps_uint4)-1) && (Server->ndocs > Server->MaxDocsPerServer) ) {
			    DpsLog(Indexer, DPS_LOG_WARN, "Maximum of %u documents per server reached, skip it.", Server->MaxDocsPerServer);
			    Doc->method = DPS_METHOD_VISITLATER;
			  }
			default:
			  break;
			}
			
		}
		DPS_FREE(alstr);
	}
/*	DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);*/
	
	
	if(result!=DPS_OK){
		DpsDocFree(Doc);
		if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}
	
	if(Doc->method != DPS_METHOD_DISALLOW && Doc->method != DPS_METHOD_VISITLATER && Indexer->Flags.cmd != DPS_IND_POPRANK) {
	  DpsDocAddDocExtraHeaders(Indexer, Doc);
	  if(!strncmp(DPS_NULL2EMPTY(Doc->CurURL.schema), "http", 4)) {
	    if(!Doc->Spider.use_robots){
	      DpsLog(Indexer,DPS_LOG_WARN, "robots.txt support is disallowed for '%s'", DPS_NULL2EMPTY(Doc->CurURL.hostinfo));
	      DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
	      result = DpsRobotParse(Indexer, NULL, NULL, DPS_NULL2EMPTY(Doc->CurURL.hostinfo));
	      DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);
	    }else{
	      DPS_ROBOT_RULE	*rule;

	      /* Check whether URL is disallowed by robots.txt */
	      rule = DpsRobotRuleFind(Indexer, Server, Doc, &Doc->CurURL, 1, (alias) ? 1 : 0);
	      if(rule) {
		DpsLog(Indexer,DPS_LOG_WARN,"SubDoc.robots.txt: '%s %s'",(rule->cmd==DPS_METHOD_DISALLOW)?"Disallow":"Allow",rule->path);
		if((rule->cmd == DPS_METHOD_DISALLOW) || (rule->cmd == DPS_METHOD_VISITLATER) )
		  Doc->method = rule->cmd;
	      }
	    }
	  }
	  if(origurl != NULL){
	    DpsVarListReplaceStr(&Doc->Sections,"URL",origurl);
	    DpsVarListDel(&Doc->Sections, "E_URL");
	    DpsVarListDel(&Doc->Sections, "URL_ID");
	    DpsURLParse(&Doc->CurURL,origurl);
	  }
	}
	
	if(result!=DPS_OK){
	        DPS_FREE(origurl); DPS_FREE(aliasurl);
		DpsDocFree(Doc);
		if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}

	if (Indexer->Flags.cmd == DPS_IND_POPRANK) {
	  if(DPS_OK != (result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_PASNEO))) {
	    DpsDocFree(Doc);
	    if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
	    TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
	    DpsViolationExit(Indexer->handle, paran);
#endif
	    return result;
	  }
	}else if(Doc->method != DPS_METHOD_DISALLOW && Doc->method != DPS_METHOD_VISITLATER) {
		int	start,state;
		int	mp3type=DPS_MP3_UNKNOWN;
		
		if(!(Indexer->flags&DPS_FLAG_REINDEX)){
			const char *l = DpsVarListFindStr(&Doc->Sections, "Last-Modified", NULL);
			int prevstatus = DpsVarListFindInt(&Doc->Sections, "PrevStatus", 0);
			if ((prevstatus < 400) && (l != NULL)) DpsVarListReplaceStr(&Doc->RequestHeaders, "If-Modified-Since", l);
		}
		
		DPS_THREADINFO(Indexer, "Getting", newhref);
		
		start = (Doc->method == DPS_METHOD_CHECKMP3 || Doc->method == DPS_METHOD_CHECKMP3ONLY) ? 1 : 0;
		
		for(state=start;state>=0;state--){
			const char	*hdr=NULL;
			
			if(state==1) hdr = "bytes=0-2048";
			if(mp3type==DPS_MP3_TAG)hdr="bytes=-128";
			
			DpsVarListReplaceInt(&Doc->Sections, "Status", DPS_HTTP_STATUS_UNKNOWN);
			
			if(aliasurl != NULL) {
			  DpsVarListReplaceStr(&Doc->Sections,"URL",alias);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL, alias);
			}
				
			DpsVarListLog(Indexer,&Doc->RequestHeaders, DPS_LOG_DEBUG, "Request");
				
			if(hdr) {
			        DpsVarListAddStr(&Doc->RequestHeaders, "Range", hdr);
				DpsLog(Indexer, DPS_LOG_INFO, "Range: [%s]", hdr);
			}

			Indexer->flags |= DPS_FLAG_FROM_STORED | DPS_FLAG_REINDEX;
			result = DpsGetURL(Indexer, Doc, origurl);
			Indexer->flags = Indexer->Conf->flags;

			if(hdr) {
			        DpsVarListDel(&Doc->RequestHeaders, "Range");
			}
				
			if(origurl != NULL) {
			  DpsVarListReplaceStr(&Doc->Sections, "URL", origurl);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL,origurl);
			}
			
			if(result!=DPS_OK){
			        DPS_FREE(origurl); DPS_FREE(aliasurl);
				DpsDocFree(Doc);
				if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
				TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
				DpsViolationExit(Indexer->handle, paran);
#endif
				return result;
			}
			
			if (Server) Server->ndocs++;

/*			DpsParseHTTPResponse(Indexer, Doc);*/
			DpsDocProcessResponseHeaders(Indexer, Doc);
/*			DpsVarListLog(Indexer, &Doc->Sections, DPS_LOG_DEBUG, "Response");*/
			
			status = DpsVarListFindInt(&Doc->Sections, "Status", 0);
			
			DpsLog(Indexer, DPS_LOG_EXTRA, "Status: %d %s", status, DpsHTTPErrMsg(status));

			if(status != DPS_HTTP_STATUS_PARTIAL_OK && status != DPS_HTTP_STATUS_OK)
				break;
			
			if(state==1){	/* Needs guessing */
				if(DPS_MP3_UNKNOWN != (mp3type = DpsMP3Type(Doc))) {
					DpsVarListReplaceStr(&Doc->Sections, "Content-Type", "audio/mpeg");
					if(Doc->method == DPS_METHOD_CHECKMP3ONLY && mp3type != DPS_MP3_TAG) break;
				}
				if(Doc->method == DPS_METHOD_CHECKMP3ONLY) break;
			}
		}
		
		/* Add URL from Location: header */
		/* This is to give a chance for  */
		/* a concurent thread to take it */
		result = DpsDocStoreHrefs(Indexer, Doc);
		if(result!=DPS_OK){
		        DPS_FREE(origurl); DPS_FREE(aliasurl);
			DpsDocFree(Doc);
			if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
			TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
			DpsViolationExit(Indexer->handle, paran);
#endif
			return result;
		}
		
		/* Increment indexer's download statistics */
		Indexer->nbytes += Doc->Buf.size;
		Indexer->ndocs++;
		
		if((!Doc->Buf.content) && (status < 500)) {
			DpsLog(Indexer, DPS_LOG_ERROR, "No data received");
			status=DPS_HTTP_STATUS_SERVICE_UNAVAILABLE;
			DpsVarListReplaceInt(&Doc->Sections, "Status", status);
		}
		
		if(status == DPS_HTTP_STATUS_OK || status==DPS_HTTP_STATUS_PARTIAL_OK 
		   || status == DPS_HTTP_STATUS_MOVED_TEMPORARILY
		   || (status == DPS_HTTP_STATUS_MOVED_PARMANENTLY && Doc->subdoc > 1)
		   ) {
		   	size_t		wordnum, min_size;
			size_t	hdr_len = Doc->Buf.content - Doc->Buf.buf;
			size_t	cont_len = Doc->Buf.size - hdr_len;
			const char *cont_lang = NULL;
			int skip_too_small;
			char reason[PATH_MAX+1];
		   	
			min_size = (size_t)DpsVarListFindUnsigned(&Indexer->Vars, "MinDocSize", 0);
			skip_too_small = (cont_len < min_size);

			if (skip_too_small) {
			  Doc->method = DPS_METHOD_HEAD;
			  DpsLog(Indexer, DPS_LOG_EXTRA, "Too small content size (%d < %d), CheckOnly.", cont_len, min_size); 
			}

			DPS_THREADINFO(Indexer,"Parsing", newhref);
			
			result = DpsDocParseContent(Indexer, Doc);
			if(result!=DPS_OK){
			        DPS_FREE(origurl); DPS_FREE(aliasurl);
				DpsDocFree(Doc);
				if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
				TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
				DpsViolationExit(Indexer->handle, paran);
#endif
				return result;
			}
			/* Guesser was here */			
			cont_lang = DpsVarListFindStr(&Doc->Sections,"Content-Language","");
			
			DpsParseURLText(Indexer, Doc);
/*			DpsParseHeaders(Indexer, Doc);*/
			{
			  int m;
			  if ((m = DpsSectionFilterFind(DPS_LOG_DEBUG,&Indexer->Conf->SectionFilters,Doc,reason)) != DPS_METHOD_NOINDEX) {
			    char *subsection;
			    DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
			    if (m == DPS_METHOD_INDEX) Doc->method = DPS_METHOD_GET;
			    switch(DpsSubSectionMatchFind(DPS_LOG_DEBUG, &Indexer->Conf->SubSectionMatch, Doc, reason, &subsection)) {
			    case DPS_METHOD_TAG:
			      DpsVarListReplaceStr(&Doc->Sections, "Tag", subsection); break;
			    case DPS_METHOD_CATEGORY:
			      DpsVarListReplaceStr(&Doc->Sections, "Category", subsection); break;
			    }
			    if (Doc->method != DPS_METHOD_HREFONLY) { DpsPrepareWords(Indexer, Doc); }
			  } else Doc->method = m;
			  DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
			}
			
			/* Remove StopWords */
			DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
			for(wordnum = 0; wordnum < Doc->Words.nwords; wordnum++) {
				const dpsunicode_t	*w = Doc->Words.Word[wordnum].uword;
				size_t		wlen = Doc->Words.Word[wordnum].ulen;
				
				if(wlen > Indexer->WordParam.max_word_len ||
				   wlen < Indexer->WordParam.min_word_len ||
				   DpsStopListFind(&Indexer->Conf->StopWords, w, cont_lang ) != NULL)
				{
					Doc->Words.Word[wordnum].coord=0;
				}	
			}
			for(wordnum = 0; wordnum < Doc->CrossWords.ncrosswords; wordnum++) {
				const dpsunicode_t	*w = Doc->CrossWords.CrossWord[wordnum].uword;
				size_t		wlen = Doc->CrossWords.CrossWord[wordnum].ulen;
				
				if(wlen>Indexer->WordParam.max_word_len ||
				   wlen<Indexer->WordParam.min_word_len ||
				   DpsStopListFind(&Indexer->Conf->StopWords,w, cont_lang) != NULL)
				{
					Doc->CrossWords.CrossWord[wordnum].weight=0;
				}	
			}
			DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);
			if (Indexer->Flags.collect_links && (status == 200 || status == 206 || status == 302) )
			  if(DPS_OK != (result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_LINKS_MARKTODEL))) {
			    DpsDocFree(Doc);
			    if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
			    TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
			    DpsViolationExit(Indexer->handle, paran);
#endif
			    return result;
			  }
		}
/*		DpsExecActions(Indexer, Doc);*/
		DpsVarListLog(Indexer, &Doc->Sections, DPS_LOG_DEBUG, "Response");
	}

	if (DPS_OK == (result = DpsDocStoreHrefs(Indexer, Doc))) {
	  if ((DPS_OK == (result = DpsStoreHrefs(Indexer))) && Indexer->Flags.collect_links  
	      && (status == 200 || status == 206 || status == 302) )
	    result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_LINKS_DELETE);
	}

	if(result!=DPS_OK){
	        DPS_FREE(origurl); DPS_FREE(aliasurl);
		DpsDocFree(Doc);
		if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}

	{
	  DPS_CHARSET *parent_cs = DpsGetCharSetByID(Parent->charset_id), *doc_cs = DpsGetCharSetByID(Doc->charset_id);
	  DPS_CONV     dc_parent;
	  DPS_TEXTLIST *tlist = &Doc->TextList;
	  char *src, *dst = NULL;
	  size_t srclen = (size_t)DpsVarListFindInt(&Doc->Sections, "Content-Length", 0);
	  size_t dstlen = (size_t)DpsVarListFindInt(&Parent->Sections, "Content-Length", 0);

	  DpsVarListReplaceInt(&Parent->Sections, "Content-Length", (int)(srclen + dstlen));
	  DpsConvInit(&dc_parent, parent_cs, doc_cs, Indexer->Conf->CharsToEscape, DPS_RECODE_HTML);
	  for(i = 0; i < tlist->nitems; i++) {
	    DPS_TEXTITEM *Item = &tlist->Items[i];
	    srclen = ((Item->len) ? Item->len : (dps_strlen(Item->str)) + 1);	/* with '\0' */
	    dstlen = (16 * (srclen + 1)) * sizeof(char);	/* with '\0' */
	    if ((dst = (char*)DpsRealloc(dst, dstlen + 1)) == NULL) {
	      DPS_FREE(origurl); DPS_FREE(aliasurl);
	      DpsDocFree(Doc);
	      if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);
	      TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
	      DpsViolationExit(Indexer->handle, paran);
#endif
	      return DPS_ERROR;
	    }
	    src = Item->str;
	    DpsConv(&dc_parent, dst, dstlen, src, srclen);
	    Item->str = dst;
/*	    fprintf(stderr, "Section: %s [%d] = %s\n", Item->section_name, Item->section, Item->str);*/
	    DpsTextListAdd(&Parent->TextList, Item);
	    Item->str = src;
	  }
	  DPS_FREE(dst);
	  
	}
	Parent->sd_cnt += 1 + Doc->sd_cnt;
/*
	for (i = 0; i < Doc->Words.nwords; i++) {
	  DpsWordListAdd(Parent, &Doc->Words.Word[i], DPS_WRDSEC(Doc->Words.Word[i].coord));
	}
	for (i = 0; i < Doc->CrossWords.ncrosswords; i++) {
	  DpsCrossListAdd(Parent, &Doc->CrossWords.CrossWord[i]);
	}
*/	
	DpsDocFree(Doc);
	if (base) DpsURLFree(baseURL); DpsURLFree(newURL); DPS_FREE(newhref);

	DPS_FREE(origurl); DPS_FREE(aliasurl);
	DPS_GETLOCK(Indexer, DPS_LOCK_THREAD);
	if (result == DPS_OK) result = Indexer->action;
	DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
	TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
	DpsViolationExit(Indexer->handle, paran);
#endif
	return result;
}


__C_LINK int __DPSCALL DpsIndexNextURL(DPS_AGENT *Indexer){
	int		result=DPS_OK, status = 0;
	DPS_DOCUMENT	*Doc;
	const char	*url, *alias = NULL;
	char		*origurl = NULL, *aliasurl = NULL;
	DPS_SERVER	*Server = NULL;
#ifdef WITH_PARANOIA
	void * paran = DpsViolationEnter(paran);
#endif

	TRACE_IN(Indexer, "DpsIndexNextURL");
	
	Doc = DpsDocInit(NULL);
	
	DPS_THREADINFO(Indexer,"Selecting","");

	if(DPS_OK==(result=DpsStoreHrefs(Indexer))) {
	        if (Indexer->action != DPS_OK && Indexer->action != DPS_NOTARGET) {
		  TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		  DpsViolationExit(Indexer->handle, paran);
#endif
		  return Indexer->action;
		}
		result = DpsNextTarget(Indexer, Doc);
	}
	
	if(result==DPS_NOTARGET){
		DpsDocFree(Doc);	/* To free Doc.connp->connp */
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}
	
	if(result!=DPS_OK){
		DpsDocFree(Doc);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}
	
	url = DpsVarListFindStr(&Doc->Sections, "URL", "");
	DpsVarListReplaceInt(&Doc->Sections, "crc32old", DpsVarListFindInt(&Doc->Sections, "crc32", 0));
	DpsLog(Indexer, (Indexer->Flags.cmd != DPS_IND_POPRANK) ? DPS_LOG_INFO : DPS_LOG_DEBUG, "URL: %s", url);
	
	/* To see the URL being indexed in "ps" output on xBSD */
	if (Indexer->Flags.cmd != DPS_IND_POPRANK) dps_setproctitle("[%d] URL:%s", Indexer->handle, url);
	
	/* Collect information from Conf */
	
	if(Indexer->Flags.cmd != DPS_IND_POPRANK && !Doc->Buf.buf) {
		/* Alloc buffer for document */
		Doc->Buf.max_size = (size_t)DpsVarListFindInt(&Indexer->Vars, "MaxDocSize", DPS_MAXDOCSIZE);
		Doc->Buf.allocated_size = DPS_NET_BUF_SIZE;
		if ((Doc->Buf.buf = (char*)DpsRealloc(Doc->Buf.buf, Doc->Buf.allocated_size + 1)) == NULL) {
		  DpsLog(Indexer, DPS_LOG_ERROR, "Out of memory (%d bytes) %s:%d", Doc->Buf.allocated_size + 1, __FILE__, __LINE__);
		  DpsDocFree(Doc);
		  TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		  DpsViolationExit(Indexer->handle, paran);
#endif
		  return DPS_ERROR;
		}
		Doc->Buf.buf[0]='\0';
	}
	
	/* Check that URL has valid syntax */
	if(DpsURLParse(&Doc->CurURL, url)) {
		DpsLog(Indexer,DPS_LOG_WARN,"Invalid URL: %s",url);
		Doc->method = DPS_METHOD_DISALLOW;
	}else
	if ((Doc->CurURL.filename != NULL) && (!strcmp(Doc->CurURL.filename, "robots.txt"))) {
		Doc->method = DPS_METHOD_DISALLOW;
	}else if (Indexer->Flags.cmd != DPS_IND_POPRANK) {
		char		*alstr = NULL;

		Doc->CurURL.charset_id = Doc->charset_id;
		/* Find correspondent Server */
/*		DPS_GETLOCK(Indexer,DPS_LOCK_CONF);*/
		Server = DpsServerFind(Indexer, (urlid_t)DpsVarListFindInt(&Doc->Sections, "Server_id", 0), url, Doc->charset_id, &alstr);
/*		DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);*/
		if ( !Server ) {
			DpsLog(Indexer,DPS_LOG_WARN,"No 'Server' command for url");
			Doc->method = DPS_METHOD_DISALLOW;
		}else{
		        DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
			Doc->lcs = Indexer->Conf->lcs;
			DpsVarList2Doc(Doc, Server);
			Doc->Spider.ExpireAt = Server->ExpireAt;
			Doc->Server = Server;
			
			DpsDocAddConfExtraHeaders(Indexer->Conf, Doc);
			DpsDocAddServExtraHeaders(Server, Doc);
			DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);

			DpsVarListReplaceLst(&Doc->Sections, &Server->Vars,NULL,"*");
/*			DpsVarListReplaceInt(&Doc->Sections, "Site_id", DpsServerGetSiteId(Indexer, Server, Doc));*/
			DpsVarListReplaceInt(&Doc->Sections, "Server_id", Server->site_id);
			DpsVarListReplaceInt(&Doc->Sections, "MaxHops", Doc->Spider.maxhops);
			
			if(alstr != NULL) {
				/* Server Primary alias found */
				DpsVarListReplaceStr(&Doc->Sections, "Alias", alstr);
			}else{
				/* Apply non-primary alias */
				result = DpsDocAlias(Indexer, Doc);
			}

			if((alias = DpsVarListFindStr(&Doc->Sections, "Alias", NULL))) {
			  const char *u = DpsVarListFindStr(&Doc->Sections, "URL", NULL);
			  origurl = (char*)DpsStrdup(u);
			  aliasurl = (char*)DpsStrdup(alias);
			  DpsLog(Indexer,DPS_LOG_EXTRA,"Alias: '%s'", alias);
			  DpsVarListReplaceStr(&Doc->Sections, "URL", alias);
			  DpsVarListReplaceStr(&Doc->Sections, "ORIG_URL", origurl);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL, alias);
			}

			/* Check hops, network errors, filters */
			result = DpsDocCheck(Indexer, Server, Doc);
			switch(Doc->method) {
			case DPS_METHOD_GET:
			case DPS_METHOD_HEAD:
			case DPS_METHOD_HREFONLY:
			case DPS_METHOD_CHECKMP3:
			case DPS_METHOD_CHECKMP3ONLY:
			  if( (Server->MaxDocsPerServer != (dps_uint4)-1) && (Server->ndocs > Server->MaxDocsPerServer) ) {
			    DpsLog(Indexer, DPS_LOG_WARN, "Maximum of %u documents per server reached, skip it.", Server->MaxDocsPerServer);
			    Doc->method = DPS_METHOD_VISITLATER;
			  }
			  DpsVarListReplaceInt(&Doc->Sections, "Site_id", DpsServerGetSiteId(Indexer, Server, Doc));
			default:
			  break;
			}
			
		}
		DPS_FREE(alstr);
	}
/*	DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);*/
	
	
	if(result!=DPS_OK){
		DpsDocFree(Doc);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}
	
	if(Doc->method != DPS_METHOD_DISALLOW && Doc->method != DPS_METHOD_VISITLATER && Indexer->Flags.cmd != DPS_IND_POPRANK) {
	  DpsDocAddDocExtraHeaders(Indexer, Doc);
	  if(!strncmp(DPS_NULL2EMPTY(Doc->CurURL.schema), "http", 4)) {
	    if(!Doc->Spider.use_robots){
	      DpsLog(Indexer,DPS_LOG_WARN, "robots.txt support is disallowed for '%s'", DPS_NULL2EMPTY(Doc->CurURL.hostinfo));
	      DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
	      result = DpsRobotParse(Indexer, NULL, NULL, DPS_NULL2EMPTY(Doc->CurURL.hostinfo));
	      DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);
	    }else{
	      DPS_ROBOT_RULE	*rule;

	      /* Check whether URL is disallowed by robots.txt */
	      rule = DpsRobotRuleFind(Indexer, Server, Doc, &Doc->CurURL, 1, (alias) ? 1 : 0);
	      if(rule) {
		DpsLog(Indexer,DPS_LOG_WARN,"Doc.robots.txt: '%s %s'",(rule->cmd==DPS_METHOD_DISALLOW)?"Disallow":"Allow",rule->path);
		if((rule->cmd == DPS_METHOD_DISALLOW) || (rule->cmd == DPS_METHOD_VISITLATER) )
		  Doc->method = rule->cmd;
	      }
	    }
	  }
	  if(origurl != NULL){
	    DpsVarListReplaceStr(&Doc->Sections,"URL",origurl);
	    DpsVarListDel(&Doc->Sections, "E_URL");
	    DpsVarListDel(&Doc->Sections, "URL_ID");
	    DpsURLParse(&Doc->CurURL,origurl);
	  }
	}
	
	if(result!=DPS_OK){
	        DPS_FREE(origurl); DPS_FREE(aliasurl);
		DpsDocFree(Doc);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}

	if (Indexer->Flags.cmd == DPS_IND_POPRANK) {
	  if(DPS_OK != (result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_PASNEO))) {
	    DpsDocFree(Doc);
	    TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
	    DpsViolationExit(Indexer->handle, paran);
#endif
	    return result;
	  }
	}else if(Doc->method != DPS_METHOD_DISALLOW && Doc->method != DPS_METHOD_VISITLATER) {
		int	start,state;
		int	mp3type=DPS_MP3_UNKNOWN;
		
		if(!(Indexer->flags&DPS_FLAG_REINDEX)){
			const char *l = DpsVarListFindStr(&Doc->Sections, "Last-Modified", NULL);
			int prevstatus = DpsVarListFindInt(&Doc->Sections, "PrevStatus", 0);
			if ((prevstatus < 400) && (l != NULL)) DpsVarListReplaceStr(&Doc->RequestHeaders, "If-Modified-Since", l);
		}
		
		DPS_THREADINFO(Indexer,"Getting",url);
		
		start = (Doc->method == DPS_METHOD_CHECKMP3 || Doc->method == DPS_METHOD_CHECKMP3ONLY) ? 1 : 0;
		
		for(state=start;state>=0;state--){
			const char	*hdr=NULL;
			
			if(state==1) hdr = "bytes=0-2048";
			if(mp3type==DPS_MP3_TAG)hdr="bytes=-128";
			
			DpsVarListReplaceInt(&Doc->Sections, "Status", DPS_HTTP_STATUS_UNKNOWN);
			
			if(aliasurl != NULL) {
			  DpsVarListReplaceStr(&Doc->Sections,"URL",alias);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL, alias);
			}
				
			DpsVarListLog(Indexer,&Doc->RequestHeaders, DPS_LOG_DEBUG, "Request");
				
			if(hdr) {
			        DpsVarListAddStr(&Doc->RequestHeaders, "Range", hdr);
				DpsLog(Indexer, DPS_LOG_INFO, "Range: [%s]", hdr);
			}
				
			result = DpsGetURL(Indexer, Doc, origurl);

			if(hdr) {
			        DpsVarListDel(&Doc->RequestHeaders, "Range");
			}
				
			if(origurl != NULL) {
			  DpsVarListReplaceStr(&Doc->Sections, "URL", origurl);
			  DpsVarListDel(&Doc->Sections, "E_URL");
			  DpsVarListDel(&Doc->Sections, "URL_ID");
			  DpsURLParse(&Doc->CurURL,origurl);
			}
			
			if(result!=DPS_OK){
			        DPS_FREE(origurl); DPS_FREE(aliasurl);
				DpsDocFree(Doc);
				TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
				DpsViolationExit(Indexer->handle, paran);
#endif
				return result;
			}
			
			if (Server) Server->ndocs++;

/*			DpsParseHTTPResponse(Indexer, Doc);*/
			DpsDocProcessResponseHeaders(Indexer, Doc);
/*			DpsVarListLog(Indexer, &Doc->Sections, DPS_LOG_DEBUG, "Response");*/
			
			status = DpsVarListFindInt(&Doc->Sections, "Status", 0);
			
			DpsLog(Indexer, DPS_LOG_EXTRA, "Status: %d %s", status, DpsHTTPErrMsg(status));

			if(status != DPS_HTTP_STATUS_PARTIAL_OK && status != DPS_HTTP_STATUS_OK)
				break;
			
			if(state==1){	/* Needs guessing */
				if(DPS_MP3_UNKNOWN != (mp3type = DpsMP3Type(Doc))) {
					DpsVarListReplaceStr(&Doc->Sections, "Content-Type", "audio/mpeg");
					if(Doc->method == DPS_METHOD_CHECKMP3ONLY && mp3type != DPS_MP3_TAG) break;
				}
				if(Doc->method == DPS_METHOD_CHECKMP3ONLY) break;
			}
		}
		
		/* Add URL from Location: header */
		/* This is to give a chance for  */
		/* a concurent thread to take it */
		result = DpsDocStoreHrefs(Indexer, Doc);
		if(result!=DPS_OK){
		        DPS_FREE(origurl); DPS_FREE(aliasurl);
			DpsDocFree(Doc);
			TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
			DpsViolationExit(Indexer->handle, paran);
#endif
			return result;
		}
		
		/* Increment indexer's download statistics */
		Indexer->nbytes += Doc->Buf.size;
		Indexer->ndocs++;
		
		if((!Doc->Buf.content) && (status < 500)) {
			DpsLog(Indexer, DPS_LOG_ERROR, "No data received");
			status=DPS_HTTP_STATUS_SERVICE_UNAVAILABLE;
			DpsVarListReplaceInt(&Doc->Sections, "Status", status);
		}
		
		if(status == DPS_HTTP_STATUS_OK || status==DPS_HTTP_STATUS_PARTIAL_OK || status == DPS_HTTP_STATUS_MOVED_TEMPORARILY) {
		   	size_t		wordnum, min_size;
			size_t	hdr_len = Doc->Buf.content - Doc->Buf.buf;
			size_t	cont_len = Doc->Buf.size - hdr_len;
			const char *lang = NULL;
			int skip_too_small;
			char reason[PATH_MAX+1];
		   	
			min_size = (size_t)DpsVarListFindUnsigned(&Indexer->Vars, "MinDocSize", 0);
			skip_too_small = (cont_len < min_size);

			if (skip_too_small) {
			  Doc->method = DPS_METHOD_HEAD;
			  DpsLog(Indexer, DPS_LOG_EXTRA, "Too small content size (%d < %d), CheckOnly.", cont_len, min_size); 
			}

			DPS_THREADINFO(Indexer,"Parsing",url);
			
			result = DpsDocParseContent(Indexer, Doc);
			if(result!=DPS_OK){
			        DPS_FREE(origurl); DPS_FREE(aliasurl);
				DpsDocFree(Doc);
				TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
				DpsViolationExit(Indexer->handle, paran);
#endif
				return result;
			}
			/* Guesser was here */			
			lang = DpsVarListFindStr(&Doc->Sections,"Content-Language","");
			
			DpsParseURLText(Indexer, Doc);
/*			DpsParseHeaders(Indexer, Doc);*/
			{
			  int m;
			  if ((m = DpsSectionFilterFind(DPS_LOG_DEBUG,&Indexer->Conf->SectionFilters,Doc,reason)) != DPS_METHOD_NOINDEX) {
			    char *subsection;
			    DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
			    if (m == DPS_METHOD_INDEX) Doc->method = DPS_METHOD_GET;
			    switch(DpsSubSectionMatchFind(DPS_LOG_DEBUG, &Indexer->Conf->SubSectionMatch, Doc, reason, &subsection)) {
			    case DPS_METHOD_TAG:
			      DpsVarListReplaceStr(&Doc->Sections, "Tag", subsection); break;
			    case DPS_METHOD_CATEGORY:
			      DpsVarListReplaceStr(&Doc->Sections, "Category", subsection); break;
			    }
			    if (Doc->method != DPS_METHOD_HREFONLY) DpsPrepareWords(Indexer, Doc);
			  } else Doc->method = m;
			  DpsLog(Indexer, DPS_LOG_DEBUG, "%s", reason);
			}
			
			/* Remove StopWords */
			DPS_GETLOCK(Indexer,DPS_LOCK_CONF);
			for(wordnum = 0; wordnum < Doc->Words.nwords; wordnum++) {
				const dpsunicode_t	*w = Doc->Words.Word[wordnum].uword;
				size_t		wlen = Doc->Words.Word[wordnum].ulen;
				
				if(wlen > Indexer->WordParam.max_word_len ||
				   wlen < Indexer->WordParam.min_word_len ||
				   DpsStopListFind(&Indexer->Conf->StopWords, w, lang ) != NULL)
				{
					Doc->Words.Word[wordnum].coord=0;
				}	
			}
			for(wordnum = 0; wordnum < Doc->CrossWords.ncrosswords; wordnum++) {
				const dpsunicode_t	*w = Doc->CrossWords.CrossWord[wordnum].uword;
				size_t		wlen = Doc->CrossWords.CrossWord[wordnum].ulen;
				
				if(wlen>Indexer->WordParam.max_word_len ||
				   wlen<Indexer->WordParam.min_word_len ||
				   DpsStopListFind(&Indexer->Conf->StopWords,w, lang) != NULL)
				{
					Doc->CrossWords.CrossWord[wordnum].weight=0;
				}	
			}
			DPS_RELEASELOCK(Indexer,DPS_LOCK_CONF);
			if (Indexer->Flags.collect_links && (status == 200 || status == 206 || status == 302) )
			  if(DPS_OK != (result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_LINKS_MARKTODEL))) {
			    DpsDocFree(Doc);
			    TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
			    DpsViolationExit(Indexer->handle, paran);
#endif
			    return result;
			  }
		}
		DpsExecActions(Indexer, Doc);
		DpsVarListLog(Indexer, &Doc->Sections, DPS_LOG_DEBUG, "Response");
	}

	if (DPS_OK == (result = DpsDocStoreHrefs(Indexer, Doc))) {
	  if ((DPS_OK == (result = DpsStoreHrefs(Indexer))) && Indexer->Flags.collect_links  
	      && (status == 200 || status == 206 || status == 302) )
	    result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_LINKS_DELETE);
	}
	
	if(result!=DPS_OK){
	        DPS_FREE(origurl); DPS_FREE(aliasurl);
		DpsDocFree(Doc);
		TRACE_OUT(Indexer);
#ifdef WITH_PARANOIA
		DpsViolationExit(Indexer->handle, paran);
#endif
		return result;
	}
	
	if (Indexer->Flags.cmd == DPS_IND_POPRANK) DpsDocFree(Doc);
	else {
	  /* Free unnecessary information */
	  DpsHrefListFree(&Doc->Hrefs);
	  DpsVarListFree(&Doc->RequestHeaders);
	  DpsTextListFree(&Doc->TextList);
	  DPS_FREE(Doc->Buf.buf);
	  Doc->Buf.max_size = Doc->Buf.allocated_size = 0;

	  result = DpsURLAction(Indexer, Doc, DPS_URL_ACTION_FLUSH);
	}
	
	DPS_FREE(origurl); DPS_FREE(aliasurl);
	DPS_GETLOCK(Indexer, DPS_LOCK_THREAD);
	if (result == DPS_OK) result = Indexer->action;
	DPS_RELEASELOCK(Indexer, DPS_LOCK_THREAD);
	TRACE_OUT(Indexer);
#ifdef FILENCE
	DpsFilenceCheckLeaks(
#ifdef WITH_TRACE
			     Indexer->TR
#else
			     NULL
#endif
			     );
#endif
#ifdef WITH_PARANOIA
	DpsViolationExit(Indexer->handle, paran);
#endif
	return result;
}
