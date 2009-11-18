/* Copyright (C) 2003-2009 Datapark corp. All rights reserved.
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
#include "dps_proto.h"
#include "dps_services.h"
#include "dps_agent.h"
#include "dps_db.h"
#include "dps_doc.h"
#include "dps_result.h"
#include "dps_sdp.h"
#include "dps_xmalloc.h"
#include "dps_searchtool.h"
#include "dps_vars.h"
#include "dps_word.h"
#include "dps_db_int.h"
#include "dps_log.h"
#include "dps_socket.h"
#include "dps_charsetutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <errno.h>

/*
#define DEBUG_SDP
*/

/*
#define DEBUG_SEARCH
*/

ssize_t DpsSearchdSendPacket(int fd,const DPS_SEARCHD_PACKET_HEADER *hdr,const void *data){
	ssize_t nsent = 0;

	if (data == NULL) {
	  nsent = DpsSend(fd, hdr, sizeof(*hdr), 0);
	} else {
	  char *ldata = (char*)DpsMalloc(sizeof(*hdr) + hdr->len);
	  if (ldata != NULL) {
	    dps_memmove(ldata, hdr, sizeof(*hdr));
	    dps_memmove(ldata + sizeof(*hdr), data, hdr->len);
	  
	    nsent = DpsSend(fd, ldata, sizeof(*hdr) + hdr->len, 0);
	  }
	  DPS_FREE(ldata);
	}
	
	return nsent;
}


#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long) -1)
#endif

static int open_host(char *hostname,int port, int timeout)
{
	int net;
	struct hostent *host;
	struct sockaddr_in sa_in;

	bzero((char*)&sa_in,sizeof(sa_in));

	if (port){
		sa_in.sin_port= htons((u_short)port);
	}else{
		return(DPS_NET_ERROR);
	}

	if ((sa_in.sin_addr.s_addr=inet_addr(hostname)) != INADDR_NONE){
		sa_in.sin_family=AF_INET;
	}else{
		host=gethostbyname(hostname);
		if (host){
			sa_in.sin_family=host->h_addrtype;
			dps_memmove(&sa_in.sin_addr, host->h_addr, (size_t)host->h_length);
		}else{
			return(DPS_NET_CANT_RESOLVE);
		}
	}
	net = socket(AF_INET, SOCK_STREAM, 0);

	DpsSockOpt(NULL, net);

	if(connect(net, (struct sockaddr *)&sa_in, sizeof (sa_in)))
		return(DPS_NET_CANT_CONNECT);

	return(net);
}

int DpsSearchdConnect(DPS_DB *cl) {
  char port_str[16];
  struct sockaddr_in dps_addr;
  unsigned char *p = (unsigned char*)&dps_addr.sin_port;
  unsigned int ip[2];
  int res = DPS_OK;
#ifdef __irix__
  int addrlen;
#else
  socklen_t addrlen;
#endif
	
  cl->searchd[1] = open_host(cl->addrURL.hostname, cl->addrURL.port ? cl->addrURL.port : DPS_SEARCHD_PORT, 0);
  if(cl->searchd[1] <= 0) {
    cl->searchd[1] = 0;
/*		DpsLog(query, DPS_LOG_ERROR, "Can't connect to searchd at '%s:%d'", host, port);*/
    res = DPS_ERROR;
  }
  if (res == DPS_OK) {

#ifdef WITH_REVERT_CONNECTION
    /* revert connection */
 
    if((cl->searchd[0] = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
/*      DpsLog(Indexer, DPS_LOG_ERROR, "StoreD ERR socket_rv: %s", strerror(errno));*/
      close(cl->searchd[1]);
      return DPS_ERROR;
    }

    if (sizeof(port_str) != DpsRecvall(cl->searchd[1], port_str, sizeof(port_str), 360)) {
/*      DpsLog(query, DPS_LOG_ERROR, "StoreD ERR receiving port data: %s", strerror(errno));*/
      close(cl->searchd[1]);
      cl->searchd[1] = 0;
      return DPS_ERROR;
    }
    addrlen = sizeof(dps_addr);
    if (getsockname(cl->searchd[1], (struct sockaddr *)&dps_addr, &addrlen) == -1) {
/*      DpsLog(Agent, DPS_LOG_ERROR, "getsockname ERR [%d] %s  %s:%d\n", errno, strerror(errno), __FILE__, __LINE__);*/
      close(cl->searchd[1]);
/*      close(client);*/
      return DPS_ERROR;
    }
    dps_addr.sin_port = 0;
    sscanf(port_str, "%u,%u", ip, ip + 1);
    p[0] = (unsigned char)(ip[0] & 255);
    p[1] = (unsigned char)(ip[1] & 255);
/*
      DpsLog(Indexer, DPS_LOG_EXTRA, "Stored @ [%s] PORT: %s, decimal:%d", 
	     inet_ntoa(Env->dbl.db[i].stored_addr.sin_addr), port_str, ntohs(dps_addr.sin_port));
*/
    DpsSockOpt(NULL, cl->searchd[0]);

    if(connect(cl->searchd[0], (struct sockaddr *)&dps_addr, sizeof(dps_addr)) == -1) {
/*	DpsLog(query, DPS_LOG_ERROR, "StoreD ERR revert connect to %s:%d - %s", 
	       inet_ntoa(dps_addr.sin_addr), ntohs(dps_addr.sin_port), strerror(errno));*/
      return DPS_ERROR;
    }
#else
    cl->searchd[0] = cl->searchd[1];
#endif

  }
  return res;
}

void DpsSearchdClose(DPS_DB *cl) {
	DPS_SEARCHD_PACKET_HEADER hdr;
	ssize_t nsent;
	
	if(cl->searchd[1] > 0) {
		/* Send goodbye */
		hdr.cmd = DPS_SEARCHD_CMD_GOODBYE;
		hdr.len = 0;
		nsent = DpsSearchdSendPacket(cl->searchd[1], &hdr, NULL);
		dps_closesocket(cl->searchd[1]);
		cl->searchd[1] = 0;
	}
	if (cl->searchd[0] > 0) {
	  dps_closesocket(cl->searchd[0]);
	  cl->searchd[0] = 0;
	}
}

int __DPSCALL DpsResAddDocInfoSearchd(DPS_AGENT * query,DPS_DB *cl,DPS_RESULT * Res,size_t clnum){
	DPS_SEARCHD_PACKET_HEADER hdr;
	char * msg=NULL;
	size_t i; /* num=0,curnum=0;*/
	int done = 0;
	ssize_t nsent,nrecv;
	char * dinfo=NULL;
	int	rc=DPS_OK;
	char		*textbuf;
	size_t dlen = 0;
	
	TRACE_IN(query, "DpsResAddDocInfoSearchd");

	if(!Res->num_rows) { TRACE_OUT(query); return(DPS_OK); }
	
	for(i=0;i<Res->num_rows;i++){
	  size_t		ulen;
	  size_t		olen;
	  size_t		nsec, r;
	  DPS_DOCUMENT	*D=&Res->Doc[i];

	  r = (size_t) 's';
	  for(nsec = 0; nsec < D->Sections.Root[r].nvars; nsec++)
	    if (strcasecmp(D->Sections.Root[r].Var[nsec].name, "Score") == 0) D->Sections.Root[r].Var[nsec].section = 1;

#ifdef WITH_MULTIDBADDR
	  if (D->dbnum != cl->dbnum) continue;
#endif		  

	  textbuf = DpsDocToTextBuf(D, 1);
	  if (textbuf == NULL) {TRACE_OUT(query); return DPS_ERROR;}
					
	  ulen = dps_strlen(textbuf)+2;
	  olen = dlen;
	  dlen = dlen + ulen;
	  dinfo = (char*)DpsRealloc(dinfo, dlen + 1);
	  if (dinfo == NULL) {
	    DpsFree(textbuf);
	    TRACE_OUT(query);
	    return DPS_ERROR;
	  }
	  dinfo[olen] = '\0';
	  sprintf(dinfo + olen, "%s\r\n", textbuf);
	  DpsFree(textbuf);
	}

	if (dinfo == NULL) {
	    TRACE_OUT(query);
	    return DPS_OK;
	}

	hdr.cmd=DPS_SEARCHD_CMD_DOCINFO;
	hdr.len = dps_strlen(dinfo);
	
	nsent = DpsSearchdSendPacket(cl->searchd[1], &hdr, dinfo);
#ifdef DEBUG_SDP
	DpsLog(query, DPS_LOG_ERROR, "Sent DOCINFO size=%d buf=%s\n", hdr.len, dinfo);
#endif				
	
	while(!done){
		char * tok, * lt;
		nrecv = DpsRecvall(cl->searchd[0], &hdr, sizeof(hdr), 360);
		
		if(nrecv!=sizeof(hdr)){
			DpsLog(query, DPS_LOG_ERROR, "Received incomplete header from searchd (%d bytes)", (int)nrecv);
			TRACE_OUT(query);
			return(DPS_ERROR);
		}else{
#ifdef DEBUG_SDP
		  DpsLog(query, DPS_LOG_ERROR, "Received header cmd=%d len=%d\n",hdr.cmd,hdr.len);
#endif
		}
		switch(hdr.cmd){
			case DPS_SEARCHD_CMD_ERROR:
				msg=(char*)DpsMalloc(hdr.len+1); 
				if (msg == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], msg, hdr.len, 360);
				msg[nrecv]='\0';
				sprintf(query->Conf->errstr,"Searchd error: '%s'",msg);
				rc=DPS_ERROR;
				DPS_FREE(msg);
				done=1;
				break;
			case DPS_SEARCHD_CMD_MESSAGE:
				msg=(char*)DpsMalloc(hdr.len+1);
				if (msg == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], msg, hdr.len, 360);
				msg[nrecv]='\0';
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Message from searchd: '%s'\n",msg);
#endif
				DPS_FREE(msg);
				break;
			case DPS_SEARCHD_CMD_DOCINFO:
				dinfo = (char*)DpsRealloc(dinfo, hdr.len + 1);
				if (dinfo == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], dinfo, hdr.len, 360);
				dinfo[hdr.len]='\0';
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received DOCINFO size=%d buf=%s\n",hdr.len,dinfo);
#endif				
				tok = dps_strtok_r(dinfo, "\r\n", &lt, NULL);
				
				while(tok){
				  urlid_t Doc_url_id, Res_Doc_url_id;
					DPS_DOCUMENT Doc;
					
					DpsDocInit(&Doc);
					DpsDocFromTextBuf(&Doc,tok);
					Doc_url_id = (urlid_t)DpsVarListFindInt(&Doc.Sections, "DP_ID", 0);

					for(i=0;i<Res->num_rows;i++){				
#ifdef WITH_MULTIDBADDR
						if (Res->Doc[i].dbnum != cl->dbnum) continue;
#endif
						Res_Doc_url_id = (urlid_t)DpsVarListFindInt(&Res->Doc[i].Sections, "DP_ID", 0);
						if (Res_Doc_url_id == Doc_url_id) {
						  DpsDocFromTextBuf(&Res->Doc[i], tok);
						  break;
						}
					}
					tok = dps_strtok_r(NULL, "\r\n", &lt, NULL);
					DpsDocFree(&Doc);
				}
				DPS_FREE(dinfo);
				done=1;
				break;
			default:
				sprintf(query->Conf->errstr,"Unknown searchd response: cmd=%d len=%d",hdr.cmd,hdr.len);
				rc=DPS_ERROR;
				done=1;
				break;
		}
	}
	TRACE_OUT(query);
	return rc;
}

static int DpsSearchdSendWordRequest(DPS_AGENT *query, DPS_DB *cl, const char * q) {
	DPS_SEARCHD_PACKET_HEADER hdr;
	ssize_t	nsent;
	size_t  nitems =  (query->flags & DPS_FLAG_UNOCON) ? query->Conf->dbl.nitems : query->dbl.nitems;

	TRACE_IN(query, "DpsSearchdSendWordRequest");
	
	hdr.cmd = (nitems > 1) ? DPS_SEARCHD_CMD_WORDS_ALL : DPS_SEARCHD_CMD_WORDS;
	hdr.len=dps_strlen(q);
	nsent=DpsSearchdSendPacket(cl->searchd[1], &hdr, q);

	TRACE_OUT(query);
	return DPS_OK;
}


int DpsSearchdGetWordResponse(DPS_AGENT *query,DPS_RESULT *Res,DPS_DB *cl) {
	DPS_URL_CRD_DB *wrd = NULL;
	DPS_URLDATA *udt = NULL;
#ifdef WITH_REL_TRACK
	DPS_URLTRACK *trk = NULL;
#endif
	DPS_SEARCHD_PACKET_HEADER hdr;
	ssize_t	nrecv;
	char	*msg;
	int	done=0, rc = DPS_OK;
	char *wbuf, *p;
	DPS_WIDEWORDLIST *wwl;
	DPS_WIDEWORD *ww;
	size_t i;

	TRACE_IN(query, "DpsSearchdGetWordResponse");
	
	Res->total_found=0;
	
	while(!done){
	  nrecv = DpsRecvall(cl->searchd[0], &hdr, sizeof(hdr), 360);
	  if(nrecv!=sizeof(hdr)){
	    sprintf(query->Conf->errstr,"Received incomplete header from searchd (%d bytes)",(int)nrecv);
	    TRACE_OUT(query);
	    return DPS_ERROR;;
	  }
#ifdef DEBUG_SDP
	  DpsLog(query, DPS_LOG_ERROR, "Received header cmd=%d len=%d\n",hdr.cmd,hdr.len);
#endif
		switch(hdr.cmd){
			case DPS_SEARCHD_CMD_ERROR:
				msg=(char*)DpsMalloc(hdr.len+1);
				if (msg == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], msg, hdr.len, 360);
				msg[nrecv]='\0';
				sprintf(query->Conf->errstr,"Searchd error: '%s'",msg);
				rc = DPS_ERROR;
				DPS_FREE(msg);
				done=1;
				break;
			case DPS_SEARCHD_CMD_MESSAGE:
				msg=(char*)DpsMalloc(hdr.len+1);
				if (msg == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], msg, hdr.len, 360);
				msg[nrecv]='\0';
				if (strncmp(msg, "Total_found", 11) == 0) {
				  Res->total_found = (size_t)DPS_ATOI(msg + 12);
				  Res->grand_total = (size_t)DPS_ATOI(strchr(msg + 12, (int)' ') + 1);
				}
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Message from searchd: '%s'\n",msg);
#endif
				DPS_FREE(msg);
				break;
			case DPS_SEARCHD_CMD_WORDS:
				DPS_FREE(wrd);
				wrd=(DPS_URL_CRD_DB*)DpsMalloc(hdr.len + 1);
				if (wrd == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], wrd, hdr.len, 360);
				/*Res->total_found=hdr.len/sizeof(*wrd);*/
				Res->num_rows = hdr.len / sizeof(*wrd);
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received words size=%d nwrd=%d\n",hdr.len, Res->num_rows /*Res->total_found*/);
#endif
				done=1;
				break;
		        case DPS_SEARCHD_CMD_SUGGEST:
			        DPS_FREE(Res->Suggest);
				Res->Suggest = (char*)DpsMalloc(hdr.len + 1);
				if (Res->Suggest == NULL) {
				  done = 1; break;
				}
				nrecv = DpsRecvall(cl->searchd[0], Res->Suggest, hdr.len, 360);
				Res->Suggest[hdr.len] = '\0';
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received Suggest size=%d\n", hdr.len);
#endif
				break;

		        case DPS_SEARCHD_CMD_PERSITE:
			        Res->PerSite = (size_t*)DpsMalloc(hdr.len + 1);
				if (Res->PerSite == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], Res->PerSite, hdr.len, 360);
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received PerSite size=%d nwrd=%d\n",hdr.len, Res->num_rows/*Res->total_found*/);
#endif
				break;
		        case DPS_SEARCHD_CMD_DATA:
			        udt = (DPS_URLDATA*)DpsMalloc(hdr.len + 1);
				if (udt == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], udt, hdr.len, 360);
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received URLDATA size=%d nwrd=%d\n", hdr.len, Res->num_rows/*Res->total_found*/);
#endif
				break;

#ifdef WITH_REL_TRACK
		        case DPS_SEARCHD_CMD_TRACKDATA:
			        trk = (DPS_URLTRACK*)DpsMalloc(hdr.len + 1);
				if (trk == NULL) {
				  done = 1;
				  break;
				}
				nrecv = DpsRecvall(cl->searchd[0], trk, hdr.len, 360);
#ifdef DEBUG_SDP
				DpsLog(query, DPS_LOG_ERROR, "Received TRACKDATA size=%d nwrd=%d\n", hdr.len, Res->num_rows/*Res->total_found*/);
#endif
				break;
#endif

		        case DPS_SEARCHD_CMD_WITHOFFSET:
/*				Res->offset = 1;*/
				break;
		        case DPS_SEARCHD_CMD_QLC:
			        if ((p = (char *)DpsXmalloc(hdr.len + 1)) != NULL) {
				  if (DpsRecvall(cl->searchd[0], p, hdr.len, 360))  {
				    DpsVarListReplaceStr(&query->Vars, "q", p);
				  }
				}
				DPS_FREE(p);
				break;
		        case DPS_SEARCHD_CMD_WWL:
				Res->PerSite = NULL;
			        if ((wbuf = p = (char *)DpsXmalloc(hdr.len + 1)) != NULL) 
				  if (DpsRecvall(cl->searchd[0], wbuf, hdr.len, 360))  {
				    wwl = (DPS_WIDEWORDLIST *)p;
				    p += sizeof(DPS_WIDEWORDLIST);
#ifdef DEBUG_SDP
				    DpsLog(query, DPS_LOG_ERROR, "wbuf :%x, wwl: %x, p: %x hdr.len:%d\n", wbuf, wwl, p, hdr.len);
				    DpsLog(query, DPS_LOG_ERROR, "Received WWL nwords=%d nuniq=%d\n", wwl->nwords, wwl->nuniq);
#endif
/*				    DpsWideWordListFree(&Res->WWList);*/
				    for(i = 0; i < wwl->nwords; i++) {
				      ww = (DPS_WIDEWORD *)p;
				      p += sizeof(DPS_WIDEWORD);
				      ww->word = p;
#ifdef DEBUG_SDP
				      DpsLog(query, DPS_LOG_ERROR, "Word {%d}: %s\n", ww->len+1, ww->word);
#endif
				      p += ww->len + 1;
				      p += sizeof(dpsunicode_t) - ((SDPALIGN)p % sizeof(dpsunicode_t));
				      ww->uword = (dpsunicode_t*)p;
				      p += sizeof(dpsunicode_t) * (ww->ulen + 1);
				      DpsWideWordListAdd(&Res->WWList, ww, DPS_WWL_STRICT);
				    }
				    Res->WWList.nuniq = wwl->nuniq;
				    DPS_FREE(wbuf);
				  }
				break;
			default:
				sprintf(query->Conf->errstr,"Unknown searchd response: cmd=%d len=%d",hdr.cmd,hdr.len);
				rc = DPS_ERROR;
				done=1;
				break;
		}
	}
	Res->CoordList.Coords = wrd;
	Res->CoordList.Data = udt;
#ifdef WITH_REL_TRACK
	Res->CoordList.Track = trk;
#endif
	TRACE_OUT(query);
	return rc;
}

int __DPSCALL DpsFindWordsSearchd(DPS_AGENT *query, DPS_RESULT *Res, DPS_DB *searchd) {
	size_t		maxlen = 1024;
	char		*request, *edf = NULL, *e_empty = NULL;
	const char *df = DpsVarListFindStr(&query->Vars, "DateFormat", NULL);
	const char *empty = DpsVarListFindStr(&query->Vars, "empty", NULL);
	const char *qs = DpsVarListFindStr(&query->Vars, "QUERY_STRING", "");
	const char *tmplt = DpsVarListFindStr(&query->Vars, "tmplt", "");
	int		res=DPS_OK;

	TRACE_IN(query, "DpsFindWordsSearchd");

	if (df) {
	  edf = (char*)DpsMalloc(dps_strlen(df) * 10 + 1);
	  if (edf == NULL) {
		sprintf(query->Conf->errstr,"Can't allocate memory");
		TRACE_OUT(query);
		return DPS_ERROR;
	  }
	  DpsEscapeURL(edf, df);
	  maxlen += dps_strlen(edf);
	}
	if (empty) {
	  edf = (char*)DpsMalloc(dps_strlen(empty) * 10 + 1);
	  if (edf == NULL) {
		sprintf(query->Conf->errstr, "Can't allocate memory");
		TRACE_OUT(query);
		return DPS_ERROR;
	  }
	  DpsEscapeURL(e_empty, empty);
	  maxlen += dps_strlen(e_empty);
	}

	maxlen += dps_strlen(qs) + dps_strlen(tmplt) + 64;

	if (NULL==(request=(char*)DpsMalloc(maxlen))) {
		sprintf(query->Conf->errstr,"Can't allocate memory");
		DPS_FREE(edf);
		TRACE_OUT(query);
		return DPS_ERROR;
	}
	
     dps_snprintf(request, maxlen, "%s&BrowserCharset=%s&IP=%s&g-lc=%s&ExcerptSize=%s&ExcerptPadding=%s&DoExcerpt=%s&tmplt=%s%s%s%s%s%s%s&sp=%s&sy=%s",
		  qs,
		  DpsVarListFindStr(&query->Vars, "BrowserCharset", "iso-8859-1"),
		  DpsVarListFindStr(&query->Vars, "IP", "localhost"),
		  DpsVarListFindStr(&query->Vars, "g-lc", "en"),
		  DpsVarListFindStr(&query->Vars, "ExcerptSize", "256"),
		  DpsVarListFindStr(&query->Vars, "ExcerptPadding", "40"),
		  (query->Flags.do_excerpt) ? "yes" : "no",
		  tmplt,
		  (edf) ? "&DateFormat=" : "", (edf) ? edf : "",
		  (e_empty) ? "&empty=" : "", (e_empty) ? e_empty : "",
		  (searchd->label) ? "&label=" : "", (searchd->label) ? searchd->label : "",
		  DpsVarListFindStr(&query->Vars, "sp", "1"),
		  DpsVarListFindStr(&query->Vars, "sy", "1") 
		  );
	DPS_FREE(edf);
	DPS_FREE(e_empty);

	request[maxlen-1]='\0';
	res = DpsSearchdSendWordRequest(query, searchd, request);
	DPS_FREE(request);
	if (DPS_OK != res) {
	  TRACE_OUT(query);
	  return res;
	}

/*	res = DpsSearchdGetWordResponse(query, Res, searchd);   called later from DpsFind */
	
	TRACE_OUT(query);
	return res;
}


int __DPSCALL DpsSearchdCatAction(DPS_AGENT *A, DPS_CATEGORY *C, int cmd, void *db) {
	DPS_DB		*searchd = db;
	DPS_SEARCHD_PACKET_HEADER hdr;
	char *buf;
	ssize_t nsent, nrecv;
	int done = 0;
	int rc=DPS_OK;
	char *msg = NULL;
	char *dinfo = NULL;

	TRACE_IN(A, "DpsSearchdCatAction");

	hdr.cmd = DPS_SEARCHD_CMD_CATINFO;
	hdr.len = sizeof(int) + dps_strlen(C->addr) + 1;
	
	if ((buf = (char*)DpsMalloc(hdr.len + 1)) == NULL) {
	  DpsLog(A, DPS_LOG_ERROR, "Out of memory");
	  TRACE_OUT(A);
	  return DPS_ERROR;
	}

	*((int*)buf) = cmd;
	dps_strcpy(buf + sizeof(int), C->addr);

	nsent = DpsSearchdSendPacket(searchd->searchd[1], &hdr, buf);

	DPS_FREE(buf);

	while(!done) {
		char * tok, * lt;
		nrecv = DpsRecvall(searchd->searchd[0], &hdr, sizeof(hdr), 360);
		
		if(nrecv != sizeof(hdr)){
			DpsLog(A, DPS_LOG_ERROR, "Received incomplete header from searchd (%d bytes)", (int)nrecv);
			TRACE_OUT(A);
			return(DPS_ERROR);
		}else{
#ifdef DEBUG_SDP
		  DpsLog(A, DPS_LOG_ERROR, "Received header cmd=%d len=%d\n", hdr.cmd, hdr.len);
#endif
		}
		switch(hdr.cmd){
			case DPS_SEARCHD_CMD_ERROR:
				msg = (char*)DpsMalloc(hdr.len + 1);
				if (msg == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], msg, hdr.len, 360);
				msg[nrecv] = '\0';
				sprintf(A->Conf->errstr, "Searchd error: '%s'", msg);
				rc=DPS_ERROR;
				DPS_FREE(msg);
				done=1;
				break;
			case DPS_SEARCHD_CMD_MESSAGE:
				msg=(char*)DpsMalloc(hdr.len+1);
				if (msg == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], msg, hdr.len, 360);
				msg[nrecv] = '\0';
#ifdef DEBUG_SDP
				DpsLog(A, DPS_LOG_ERROR, "Message from searchd: '%s'\n",msg);
#endif
				DPS_FREE(msg);
				break;
			case DPS_SEARCHD_CMD_CATINFO:
			        dinfo=(char*)DpsMalloc(hdr.len+1);
				if (dinfo == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], dinfo, hdr.len, 360);
				dinfo[hdr.len]='\0';
#ifdef DEBUG_SDP
				DpsLog(A, DPS_LOG_ERROR, "Received CATINFO size=%d buf=%s\n",hdr.len,dinfo);
#endif				

				C->ncategories = 0;
				tok = dps_strtok_r(dinfo, "\r\n", &lt, NULL);
				
				while(tok){
					DpsCatFromTextBuf(C, tok);
					
					tok = dps_strtok_r(NULL, "\r\n", &lt, NULL);
				}
				DPS_FREE(dinfo);
				done=1;
				break;
			default:
				sprintf(A->Conf->errstr, "Unknown searchd response: cmd=%d len=%d", hdr.cmd, hdr.len);
				rc=DPS_ERROR;
				done = 1;
				break;
		}
	}
	TRACE_OUT(A);
	return rc;
}

int __DPSCALL DpsSearchdURLAction(DPS_AGENT *A, DPS_DOCUMENT *D, int cmd, void *db) {
	DPS_DB		*searchd = db;

	DPS_SEARCHD_PACKET_HEADER hdr;
	char *buf;
	ssize_t nsent, nrecv;
	int done = 0;
	char *msg = NULL;
	char *dinfo = NULL;
	int	rc=DPS_OK;

	TRACE_IN(A, "DpsSearchdURLAction");

	if (cmd != DPS_URL_ACTION_DOCCOUNT) {
	  DpsLog(A, DPS_LOG_ERROR, "searchd: unsupported URL action");
	  TRACE_OUT(A);
	  return DPS_ERROR;
	}
	
	hdr.cmd = DPS_SEARCHD_CMD_URLACTION;
	hdr.len = sizeof(int);
	
	if ((buf = (char*)DpsMalloc(hdr.len + 1)) == NULL) {
	  DpsLog(A, DPS_LOG_ERROR, "Out of memory");
	  TRACE_OUT(A);
	  return DPS_ERROR;
	}

	*((int*)buf) = cmd;

	nsent = DpsSearchdSendPacket(searchd->searchd[1], &hdr, buf);

	DPS_FREE(buf);

	while(!done) {

	  nrecv = DpsRecvall(searchd->searchd[0], &hdr, sizeof(hdr), 360);
		
		if(nrecv != sizeof(hdr)){
			DpsLog(A, DPS_LOG_ERROR, "Received incomplete header from searchd (%d bytes)", (int)nrecv);
			TRACE_OUT(A);
			return(DPS_ERROR);
		}else{
#ifdef DEBUG_SDP
			DpsLog(A, DPS_LOG_ERROR, "Received header cmd=%d len=%d\n", hdr.cmd, hdr.len);
#endif
		}
		switch(hdr.cmd){
			case DPS_SEARCHD_CMD_ERROR:
				msg = (char*)DpsMalloc(hdr.len + 1); 
				if (msg == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], msg, hdr.len, 360);
				msg[nrecv] = '\0';
				sprintf(A->Conf->errstr, "Searchd error: '%s'", msg);
				rc=DPS_OK;
				DPS_FREE(msg);
				done=1;
				break;
			case DPS_SEARCHD_CMD_MESSAGE:
				msg=(char*)DpsMalloc(hdr.len+1);
				if (msg == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], msg, hdr.len, 360);
				msg[nrecv] = '\0';
#ifdef DEBUG_SDP
				DpsLog(A, DPS_LOG_ERROR, "Message from searchd: '%s'\n",msg);
#endif
				DPS_FREE(msg);
				break;
			case DPS_SEARCHD_CMD_DOCCOUNT:
			        dinfo=(char*)DpsMalloc(hdr.len+1);
				if (dinfo == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(searchd->searchd[0], dinfo, hdr.len, 360);
				dinfo[hdr.len]='\0';

				A->doccount += *((int *)dinfo);
#ifdef DEBUG_SDP
				DpsLog(A, DPS_LOG_DEBUG, "Received DOCCOUNT size=%d doccount=%d(+%s)\n", hdr.len, A->doccount, dinfo);
#endif				
				DPS_FREE(dinfo);
				done=1;
				break;
			default:
				sprintf(A->Conf->errstr, "Unknown searchd response: cmd=%d len=%d", hdr.cmd, hdr.len);
				rc=DPS_ERROR;
				done = 1;
				break;
		}
	}
	TRACE_OUT(A);
	return rc;
}

int DpsCloneListSearchd(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc, DPS_RESULT *Res, DPS_DB *db) {
	DPS_SEARCHD_PACKET_HEADER hdr;
	ssize_t	nsent,nrecv;
	char *msg = NULL, *dinfo = NULL;
	char *tok, *lt;
	char buf[128];
	int done = 0;
	int	rc = DPS_OK;

	TRACE_IN(Indexer, "DpsCloneListSearchd");
	
	dps_snprintf(buf, 128, "%s", DpsVarListFindStr(&Doc->Sections, "DP_ID", "0"));
	hdr.cmd = DPS_SEARCHD_CMD_CLONES;
	hdr.len = dps_strlen(buf);
	nsent = DpsSearchdSendPacket(db->searchd[1], &hdr, buf);
	while(!done){
	  nrecv = DpsRecvall(db->searchd[0], &hdr, sizeof(hdr), 360);
		
		if(nrecv != sizeof(hdr)){
			DpsLog(Indexer, DPS_LOG_ERROR, "Received incomplete header from searchd (%d bytes)", (int)nrecv);
			TRACE_OUT(Indexer);
			return(DPS_ERROR);
		}else{
#ifdef DEBUG_SDP
			DpsLog(Indexer, DPS_LOG_DEBUG, "Received header cmd=%d len=%d\n", hdr.cmd, hdr.len);
#endif
		}
		switch(hdr.cmd){
			case DPS_SEARCHD_CMD_ERROR:
				msg = (char*)DpsMalloc(hdr.len + 1); 
				if (msg == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(db->searchd[0], msg, hdr.len, 360);
				msg[nrecv] = '\0';
				sprintf(Indexer->Conf->errstr, "Searchd error: '%s'", msg);
				rc = DPS_ERROR;
				DPS_FREE(msg);
				done = 1;
				break;
			case DPS_SEARCHD_CMD_DOCINFO:
				dinfo = (char*)DpsMalloc(hdr.len + 1);
				if (dinfo == NULL) {
				  done=1;
				  break;
				}
				nrecv = DpsRecvall(db->searchd[0], dinfo, hdr.len, 360);
				dinfo[hdr.len] = '\0';
#ifdef DEBUG_SDP
				DpsLog(Indexer, DPS_LOG_DEBUG, "Received DOCINFO size=%d buf=%s\n", hdr.len, dinfo);
#endif				
				if (strcasecmp(dinfo, "nocloneinfo") != 0) {

				  tok = dps_strtok_r(dinfo, "\r\n", &lt, NULL);
				
				  while(tok){
					DPS_DOCUMENT *D;
					size_t nd = Res->num_rows++;

					Res->Doc = (DPS_DOCUMENT*)DpsRealloc(Res->Doc, (Res->num_rows + 1) * sizeof(DPS_DOCUMENT));
					if (Res->Doc == NULL) {
					  sprintf(Indexer->Conf->errstr, "Realloc error");
					  rc = DPS_ERROR;
					  break;
					}
					D = &Res->Doc[nd];
					DpsDocInit(D);
					DpsDocFromTextBuf(D, tok);
					tok = dps_strtok_r(NULL, "\r\n", &lt, NULL);
				  }
				}
				DPS_FREE(dinfo);
				done = 1;
				break;
			default:
				sprintf(Indexer->Conf->errstr, "Unknown searchd response: cmd=%d len=%d", hdr.cmd, hdr.len);
				rc = DPS_ERROR;
				done = 1;
				break;
		}
	}
	TRACE_OUT(Indexer);
	return rc;
}
