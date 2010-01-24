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
#include "dps_db.h"
#include "dps_sqldbms.h"
#include "dps_agent.h"
#include "dps_env.h"
#include "dps_conf.h"
#include "dps_services.h"
#include "dps_sdp.h"
#include "dps_log.h"
#include "dps_xmalloc.h"
#include "dps_doc.h"
#include "dps_result.h"
#include "dps_searchtool.h"
#include "dps_vars.h"
#include "dps_match.h"
#include "dps_spell.h"
#include "dps_mutex.h"
#include "dps_signals.h"
#include "dps_socket.h"
#include "dps_store.h"
#include "dps_hash.h"
#include "dps_charsetutils.h"
#include "dps_searchcache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <signal.h>
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <fcntl.h>
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_SYS_MSG_H
#include <sys/msg.h>
#endif
#ifdef HAVE_SYS_IPC_H
#include <sys/ipc.h>
#endif

/* This should be last include */
#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)-1)
#endif

#ifdef O_BINARY
#define DPS_BINARY O_BINARY
#else
#define DPS_BINARY 0
#endif

/*
#define DEBUG_SEARCH 1
*/

static	fd_set mask;
static DPS_CHILD Children[DPS_CHILDREN_LIMIT];
static int clt_sock;
static int my_generation = 0;
static pid_t parent_pid = 0;
static size_t MaxClients = 1;

/********************* SIG Handlers ****************/
static void sighandler(int sign);
static void TrackSighandler(int sign);
static void init_signals(void){
	/* Set up signals handler*/
	DpsSignal(SIGPIPE, sighandler);
	DpsSignal(SIGCHLD, sighandler);
	DpsSignal(SIGALRM, sighandler);
	DpsSignal(SIGUSR1, sighandler);
	DpsSignal(SIGUSR2, sighandler);
	DpsSignal(SIGHUP, sighandler);
	DpsSignal(SIGINT, sighandler);
	DpsSignal(SIGTERM, sighandler);
}

static void sighandler(int sign){
        pid_t chpid;
#ifdef UNIONWAIT
	union wait status;
#else
	int status;
#endif
	switch(sign){
		case SIGPIPE:
			have_sigpipe = 1;
			break;
		case SIGCHLD:
		        while ((chpid = waitpid(-1, &status, WNOHANG)) > 0) {
			  register size_t i;
			  for (i = 0; i < MaxClients; i++) {
			    if (chpid == Children[i].pid) Children[i].pid = 0;
			  }
			}
			break;
		case SIGHUP:
			have_sighup=1;
			break;
		case SIGINT:
			have_sigint=1;
			break;
		case SIGTERM:
			have_sigterm=1;
			break;
	        case SIGALRM:
		        _exit(0);
			break;
		case SIGUSR1:
		  have_sigusr1 = 1;
		  break;
		case SIGUSR2:
		  have_sigusr2 = 1;
		  break;
		default:
			break;
	}
	init_signals();
}

static void init_TrackSignals(void){
	/* Set up signals handler*/
	DpsSignal(SIGPIPE, TrackSighandler);
	DpsSignal(SIGCHLD, TrackSighandler);
	DpsSignal(SIGALRM, TrackSighandler);
	DpsSignal(SIGUSR1, TrackSighandler);
	DpsSignal(SIGUSR2, TrackSighandler);
	DpsSignal(SIGHUP, TrackSighandler);
	DpsSignal(SIGINT, TrackSighandler);
	DpsSignal(SIGTERM, TrackSighandler);
}

static void TrackSighandler(int sign){
#ifdef UNIONWAIT
	union wait status;
#else
	int status;
#endif
	switch(sign){
		case SIGPIPE:
			have_sigpipe = 1;
			break;
		case SIGCHLD:
			while (waitpid(-1, &status, WNOHANG) > 0);
			break;
		case SIGHUP:
			have_sighup=1;
			break;
		case SIGINT:
			have_sigint=1;
			break;
		case SIGTERM:
			have_sigterm=1;
			break;
	        case SIGALRM:
		        _exit(0);
			break;
		case SIGUSR1:
		  have_sigusr1 = 1;
		  break;
		case SIGUSR2:
		  have_sigusr2 = 1;
		  break;
		default:
			break;
	}
	init_signals();
}

/*************************************************************/


static int do_client(DPS_AGENT *Agent, int client){
	char buf[1024]="";
	DPS_SEARCHD_PACKET_HEADER hdr;
	DPS_RESULT  *Res;
	struct	sockaddr_in server_addr;
	struct	sockaddr_in his_addr;
	struct	in_addr bind_address;
	char port_str[16];
	char *words = NULL;
	ssize_t nrecv,nsent;
	int verb=-1;
	int done=0;
	int		page_number;
	int		page_size;
	int pre_server, server;
	const char *bcharset;
	unsigned char *p;
	size_t ExcerptSize, ExcerptPadding;
#ifdef __irix__
	int addrlen;
#else
	socklen_t addrlen;
#endif
#ifdef DEBUG_SEARCH
	unsigned long ticks;
	unsigned long total_ticks = ticks = DpsStartTimer();
#else
	unsigned long ticks = DpsStartTimer();
#endif

	Res=DpsResultInit(NULL);
	if (Res == NULL) return DPS_ERROR;

#ifdef WITH_REVERT_CONNECTION
	/* revert connection */

	bind_address.s_addr 	= htonl(INADDR_ANY);
	server_addr.sin_family	= AF_INET;
	server_addr.sin_addr	= bind_address;
	server_addr.sin_port	= 0; /* any free port */
	p = (unsigned char*) &server_addr.sin_port;

	if ((pre_server = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  DpsLog(Agent, DPS_LOG_ERROR, "socket() ERR: %d %s", errno, strerror(errno));
	  close(client);
	  return DPS_ERROR;
	}
	DpsSockOpt(Agent, pre_server);
	if (bind(pre_server, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
	  DpsLog(Agent, DPS_LOG_ERROR, "bind() ERR: %d %s", errno, strerror(errno));
	  close(pre_server);
	  close(client);
	  return DPS_ERROR;
	}
	if (listen(pre_server, 1) < 0) {
	  DpsLog(Agent, DPS_LOG_ERROR, "listen() ERR: %d %s\n", errno, strerror(errno));
	  close(pre_server);
	  close(client);
	  return DPS_ERROR;
	}
	addrlen = sizeof(server_addr);
	if (getsockname(pre_server, (struct sockaddr *)&server_addr, &addrlen) == -1) {
	  DpsLog(Agent, DPS_LOG_ERROR, "getsockname ERR [%d] %s  %s:%d\n", errno, strerror(errno), __FILE__, __LINE__);
	  close(pre_server);
	  close(client);
	  return DPS_ERROR;
	}
	dps_snprintf(port_str, 15, "%d,%d", p[0], p[1]);

	nsent = DpsSend(client, port_str, sizeof(port_str), 0);

	if (nsent != sizeof(port_str)) {
	  DpsLog(Agent, DPS_LOG_ERROR, "ERR port sent %d of %d bytes\n", nsent, sizeof(port_str));
	  close(pre_server);
	  close(client);
	  return DPS_ERROR;
	}
		  
	bzero((void*)&his_addr, addrlen = sizeof(his_addr));
	if ((server = accept(pre_server, (struct sockaddr *)&his_addr, &addrlen)) <= 0) {
	  DpsLog(Agent, DPS_LOG_ERROR, "revert accept ERR on port %d error %d %s\n", ntohs(server_addr.sin_port), errno, strerror(errno));
	  close(pre_server);
	  close(client);
	  return DPS_ERROR;
	}
	DpsLog(Agent, DPS_LOG_INFO, "[%s] Connected. PORT: %s", inet_ntoa(his_addr.sin_addr), port_str);
	close(pre_server);
#else
	server = client;
#endif

	while(!done){
	  size_t dlen = 0, ndocs, i, last_cmd;
		int * doc_id=NULL;
		char * dinfo=NULL;
		char * tok, * lt;
		
		DpsLog(Agent,verb,"Waiting for command header");
		nrecv = DpsRecvall(client, &hdr, sizeof(hdr), 60);
		if(nrecv != sizeof(hdr)){
			DpsLog(Agent,verb,"Received incomplete header nrecv=%d", (int)nrecv);
			break;
		}else{
			DpsLog(Agent,verb,"Received header cmd=%d len=%d", hdr.cmd, hdr.len);
		}
		switch(last_cmd = hdr.cmd){
			case DPS_SEARCHD_CMD_DOCINFO:
				dinfo = (char*)DpsRealloc(dinfo, hdr.len + 1);
				if (dinfo == NULL) {
					DPS_FREE(doc_id);
					done=1;
					break;
				}
				nrecv = DpsRecvall(client, dinfo, hdr.len, 360);
				if((size_t)nrecv != hdr.len){
					DpsLog(Agent,verb,"Received incomplete data nbytes=%d nrecv=%d",hdr.len,(int)nrecv);
					DPS_FREE(doc_id);
					done=1;
					break;
				}
				dinfo[hdr.len]='\0';
				ndocs = 0;
				tok = dps_strtok_r(dinfo, "\r\n", &lt, NULL);
				
				while(tok){
					Res->Doc = (DPS_DOCUMENT*)DpsRealloc(Res->Doc, sizeof(DPS_DOCUMENT) * (ndocs + 1));
					if (Res->Doc == NULL) {
					  DPS_FREE(doc_id);
					  done=1;
					  break;
					}
					DpsDocInit(&Res->Doc[ndocs]);
					DpsVarListReplaceLst(&Res->Doc[ndocs].Sections, &Agent->Conf->Sections, NULL, "*");
					Res->Doc[ndocs].method = DPS_METHOD_GET;
					DpsDocFromTextBuf(&Res->Doc[ndocs], tok);
#ifdef WITH_MULTIDBADDR
					if ((Agent->flags & DPS_FLAG_UNOCON) ? (Agent->Conf->dbl.nitems > 1) : (Agent->dbl.nitems > 1)) {
					  char *dbstr = DpsVarListFindStr(&Res->Doc[ndocs].Sections, "dbnum", NULL);
					  if (dbstr != NULL) {
					    Res->Doc[ndocs].dbnum = DPS_ATOI(dbstr);
					  }
					}
#endif
					
					tok = dps_strtok_r(NULL, "\r\n", &lt, NULL);
					ndocs++;
				}
				
				Res->num_rows = ndocs;
				DpsLog(Agent,verb,"Received DOCINFO command len=%d ndocs=%d",hdr.len,ndocs);
#ifdef DEBUG_SEARCH
				total_ticks = DpsStartTimer();
#endif
				
				if(DPS_OK != DpsResAction(Agent, Res, DPS_RES_ACTION_DOCINFO)){
/*					DpsResultFree(Res);  must not be here */
					dps_snprintf(buf,sizeof(buf)-1,"%s",DpsEnvErrMsg(Agent->Conf));
					DpsLog(Agent,verb,"%s",DpsEnvErrMsg(Agent->Conf));
					hdr.cmd=DPS_SEARCHD_CMD_ERROR;
					hdr.len=dps_strlen(buf);
					nsent = DpsSearchdSendPacket(server, &hdr, buf);
					done=1;
					break;
				}
				
#ifdef DEBUG_SEARCH
				total_ticks = DpsStartTimer() - total_ticks;
				DpsLog(Agent, DPS_LOG_EXTRA, "ResAction in %.2f sec.", (float)total_ticks / 1000);
#endif
				dlen=0;
				
#ifdef DEBUG_SEARCH
				total_ticks = DpsStartTimer();
#endif
				DpsAgentStoredConnect(Agent);
				ExcerptSize = (size_t)DpsVarListFindInt(&Agent->Vars, "ExcerptSize", 256);
				ExcerptPadding = (size_t)DpsVarListFindInt(&Agent->Vars, "ExcerptPadding", 40);
	

				for(i=0;i<Res->num_rows;i++){
					size_t		ulen;
					size_t		olen;
					char		*textbuf, *Excerpt = NULL, *al;
					size_t		nsec, r;
					DPS_DOCUMENT	*D=&Res->Doc[i];
					
					al = DpsVarListFindStrTxt(&D->Sections, "URL", "");
					DpsLog(Agent, DPS_LOG_DEBUG, "Start excerpts for %s [dbnum:%d]", al, D->dbnum);

					if (Agent->Flags.do_excerpt) Excerpt = DpsExcerptDoc(Agent, Res, D, ExcerptSize, ExcerptPadding);

					if ((Excerpt != NULL) && (dps_strlen(Excerpt) > 6)) {
					  DpsVarListReplaceStr(&D->Sections, "body", Excerpt);
					}
					if (DpsVarListFindStr(&D->Sections, "Z", NULL) != NULL) {
					  DpsVarListReplaceStr(&D->Sections, "ST", "0");
					}
					DPS_FREE(Excerpt);
/*
					for (r = 0; r < 256; r++)
					for (nsec = 0; nsec < D->Sections.Root[r].nvars; nsec++)
						D->Sections.Root[r].Var[nsec].section = 1;
*/					
					textbuf = DpsDocToTextBuf(D, 1);
					if (textbuf == NULL) break;
/*					
					fprintf(stderr, "%s\n\n", textbuf);
*/
					ulen=dps_strlen(textbuf)+2;
					olen=dlen;
					dlen=dlen+ulen;
					dinfo=(char*)DpsRealloc(dinfo,dlen+1);
					if (dinfo == NULL) {
					  DPS_FREE(doc_id);
					  done=1;
					  break;
					}
					dinfo[olen]='\0';
					sprintf(dinfo+olen,"%s\r\n",textbuf);
					DpsFree(textbuf);
				}
				
#ifdef DEBUG_SEARCH
				total_ticks = DpsStartTimer() - total_ticks;
				DpsLog(Agent, DPS_LOG_EXTRA, "Excerpts in %.2f sec.", (float)total_ticks / 1000);
#endif
				if(!dinfo) dinfo = (char*)DpsStrdup("nodocinfo");
			
				hdr.cmd=DPS_SEARCHD_CMD_DOCINFO;
				hdr.len=dps_strlen(dinfo);
				nsent = DpsSearchdSendPacket(server, &hdr, dinfo);
				DpsLog(Agent, verb, "Sent doc_info packet %d bytes", (int)nsent);
				DPS_FREE(dinfo);
				
				break;

		        case DPS_SEARCHD_CMD_CLONES:
			  {
			    DPS_RESULT *Cl;
			    DPS_DOCUMENT *D;
			    int origin_id;
			    char cl_buf[128];

			    if (hdr.len > 128) {
			      DpsLog(Agent, verb, "Received too many data bytes=%d", hdr.len);
			      done = 1;
			      break;
			    }
			    nrecv = DpsRecvall(client, cl_buf, hdr.len, 360);
			    if((size_t)nrecv != hdr.len){
			      DpsLog(Agent, verb, "Received incomplete data nbytes=%d nrecv=%d", hdr.len, (int)nrecv);
			      done = 1;
			      break;
			    }
			    cl_buf[nrecv] = '\0';
			    sscanf(cl_buf, "%d", &origin_id);
			    D = DpsDocInit(NULL);
			    DpsVarListReplaceLst(&D->Sections, &Agent->Conf->Sections, NULL, "*");
			    D->method = DPS_METHOD_GET;
			    DpsVarListAddInt(&D->Sections, "DP_ID", origin_id);
			    Cl = DpsCloneList(Agent, &Agent->Vars, D);

			    DpsDocFree(D);
			    dlen=0;
				
			    if (Cl) for(i = 0; i < Cl->num_rows; i++) {
					size_t		ulen;
					size_t		olen;
					char		*textbuf;
					size_t		nsec, r;
					
					D = &Cl->Doc[i];
					
					for (r = 0; r < 256; r++)
					for (nsec = 0; nsec < D->Sections.Root[r].nvars; nsec++)
						D->Sections.Root[r].Var[nsec].section = 1;
					
					textbuf = DpsDocToTextBuf(D, 1);
					if (textbuf == NULL) break;
					
					ulen=dps_strlen(textbuf)+2;
					olen=dlen;
					dlen=dlen+ulen;
					dinfo=(char*)DpsRealloc(dinfo,dlen+1);
					if (dinfo == NULL) {
					  DPS_FREE(doc_id);
					  done=1;
					  break;
					}
					dinfo[olen]='\0';
					sprintf(dinfo+olen,"%s\r\n",textbuf);
					DpsFree(textbuf);
			    }
			    
			    if (!dinfo) dinfo = (char*)DpsStrdup("nocloneinfo");
				
			    hdr.cmd = DPS_SEARCHD_CMD_DOCINFO;
			    hdr.len = dps_strlen(dinfo); 
			    nsent = DpsSearchdSendPacket(server, &hdr, dinfo);
			    DpsLog(Agent, verb, "Sent clone_info packet %d bytes", (int)nsent);
			    DPS_FREE(dinfo);
			    DpsResultFree(Cl);
			  }				
			  break;

 		        case DPS_SEARCHD_CMD_CATINFO:
			  {
			        DPS_CATEGORY Cat;
				int cmd;
				
				bzero((void*)&Cat, sizeof(DPS_CATEGORY));
				nrecv = DpsRecvall(client, &cmd, sizeof(int), 360);
				nrecv = DpsRecvall(client, Cat.addr, hdr.len - sizeof(int), 360);
				Cat.addr[hdr.len - sizeof(int)] = '\0';
				DpsLog(Agent,verb,"Received CATINFO command len=%d, cmd=%d, addr='%s'", hdr.len, cmd, Cat.addr);

				DpsCatAction(Agent, &Cat, cmd);
				
				dlen = Cat.ncategories * 1024;
				dinfo = (char*)DpsMalloc(dlen + 1);
				if (dinfo == NULL) {
					DPS_FREE(doc_id);
					done=1;
					break;
				}
				DpsCatToTextBuf(&Cat, dinfo, dlen);
				
				hdr.cmd = DPS_SEARCHD_CMD_CATINFO;
				hdr.len = dps_strlen(dinfo);
				nsent = DpsSearchdSendPacket(server, &hdr, dinfo);
				DpsLog(Agent,verb,"Sent cat_info packet %d bytes",(int)nsent);
				DPS_FREE(dinfo);

				DPS_FREE(Cat.Category);
				
			  }
			  break;
				
 		        case DPS_SEARCHD_CMD_URLACTION:
			  {
				int cmd;
				
				DpsLog(Agent,verb,"Received URLACTION command len=%d", hdr.len);
				nrecv = DpsRecvall(client, &cmd, sizeof(int), 360);

				DpsURLAction(Agent, NULL, cmd);
				DpsLog(Agent,verb,"Received URLACTION command len=%d", hdr.len);
				
				hdr.cmd = DPS_SEARCHD_CMD_DOCCOUNT;
				hdr.len = sizeof(Agent->doccount);
				nsent = DpsSearchdSendPacket(server, &hdr, (char*)&Agent->doccount);
				DpsLog(Agent,verb,"Sent doccount packet %d bytes",(int)nsent);

			  }
			  break;
				
			case DPS_SEARCHD_CMD_WORDS:
		        case DPS_SEARCHD_CMD_WORDS_ALL:
			        words = (char*)DpsRealloc(words, hdr.len + 1);
				if (words == NULL) {
					dps_snprintf(buf, sizeof(buf)-1, "Can't alloc memory for query");
					DpsLog(Agent, verb, "Can't alloc memory for query");
					hdr.cmd=DPS_SEARCHD_CMD_ERROR;
					hdr.len=dps_strlen(buf);
					DpsSearchdSendPacket(server, &hdr, buf);
					done=1;
					break;
				}
				nrecv=DpsRecvall(client, words, hdr.len, 360); /* FIXME: check */
				words[nrecv]='\0';
				DpsLog(Agent,verb,"Received words len=%d words='%s'",nrecv,words);
				
				DpsParseQueryString(Agent, &Agent->Vars, words);
				bcharset = DpsVarListFindStr(&Agent->Vars, "BrowserCharset", "iso-8859-1");
				if (!(Agent->Conf->bcs = DpsGetCharSet(bcharset))) {
					dps_snprintf(buf,sizeof(buf)-1,"Unknown BrowserCharset: %s", bcharset);
					DpsLog(Agent,verb,"Unknown BrowserCharset: %s", bcharset);
					hdr.cmd=DPS_SEARCHD_CMD_ERROR;
					hdr.len=dps_strlen(buf);
					DpsSearchdSendPacket(server, &hdr, buf);
					done=1;
					break;
				}
				
				DpsLog(Agent, verb, "Query: %s [Charset: %s]", 
					DpsVarListFindStr(&Agent->Vars, "q", ""), bcharset);
				
				DpsPrepare(Agent, Res);		/* Prepare search query */
#ifdef HAVE_ASPELL
				if (Agent->Flags.use_aspellext && Agent->naspell > 0) {
				  register size_t z;
/*#ifdef UNIONWAIT
				  union wait status;
#else
				  int status;
#endif*/
				  for (z = 0; z < Agent->naspell; z++) {
				    if (Agent->aspell_pid[z]) {
				      kill(Agent->aspell_pid[z], SIGALRM);
				      Agent->aspell_pid[z] = 0;
				    }
				  }
/*				  while(waitpid(Agent->aspell_pid, &status, WNOHANG) > 0); we catch it by signal */
				  Agent->naspell = 0;
				}
#endif /* HAVE_ASPELL */
				if(DPS_OK != DpsFindWords(Agent, Res)) {
					dps_snprintf(buf,sizeof(buf)-1,"%s",DpsEnvErrMsg(Agent->Conf));
					DpsLog(Agent,verb,"%s",DpsEnvErrMsg(Agent->Conf));
					hdr.cmd=DPS_SEARCHD_CMD_ERROR;
					hdr.len = dps_strlen(buf) + 1;
					DpsSearchdSendPacket(server, &hdr, buf);
					done=1;
					break;
				}
				
				dps_snprintf(buf,sizeof(buf)-1,"Total_found=%d %d", Res->total_found, Res->grand_total);
				hdr.cmd=DPS_SEARCHD_CMD_MESSAGE;
				hdr.len = dps_strlen(buf) + 1;
				nsent=DpsSearchdSendPacket(server, &hdr, buf);
				DpsLog(Agent,verb,"Sent total_found packet %d bytes buf='%s'",(int)nsent,buf);

/*				if (Res->offset) {
				  hdr.cmd = DPS_SEARCHD_CMD_WITHOFFSET;
				  hdr.len = 0;
				  nsent = DpsSearchdSendPacket(server, &hdr, buf);
				  DpsLog(Agent,verb,"Sent withoffset packet %d bytes",(int)nsent);
				}*/

				{
				  char *wbuf, *p;
				  p = DpsVarListFindStr(&Agent->Vars, "q", "");
				  hdr.cmd = DPS_SEARCHD_CMD_QLC;
				  hdr.len = dps_strlen(p);
				  nsent = DpsSearchdSendPacket(server, &hdr, p);

				  hdr.cmd = DPS_SEARCHD_CMD_WWL;
				  hdr.len = sizeof(DPS_WIDEWORDLIST);
				  for (i = 0; i < Res->WWList.nwords; i++) {
				    hdr.len += sizeof(DPS_WIDEWORD) 
				      + sizeof(char) * (Res->WWList.Word[i].len + 1) 
				      + sizeof(dpsunicode_t) * (Res->WWList.Word[i].ulen + 1) 
				      + sizeof(int);
				  }
				  if (hdr.len & 1) hdr.len++;
				  p = wbuf = (char *)DpsXmalloc(hdr.len + 1); /* need DpsXmalloc */
				  if (p == NULL) {
					done=1;
					break;
				  }
				  dps_memmove(p, &(Res->WWList), sizeof(DPS_WIDEWORDLIST));
				  p += sizeof(DPS_WIDEWORDLIST);
				  for (i = 0; i < Res->WWList.nwords; i++) {
				    dps_memmove(p, &(Res->WWList.Word[i]), sizeof(DPS_WIDEWORD));
				    p += sizeof(DPS_WIDEWORD);
				    dps_memmove(p, Res->WWList.Word[i].word, Res->WWList.Word[i].len + 1);
				    p += Res->WWList.Word[i].len + 1;
				    p += sizeof(dpsunicode_t) - ((SDPALIGN)p % sizeof(dpsunicode_t));
				    dps_memmove(p, Res->WWList.Word[i].uword, sizeof(dpsunicode_t) * (Res->WWList.Word[i].ulen + 1));
				    p += sizeof(dpsunicode_t) * (Res->WWList.Word[i].ulen + 1);
				  }
				  nsent = DpsSearchdSendPacket(server, &hdr, wbuf);
				  DpsLog(Agent, verb, "Sent WWL packet %d bytes cmd=%d len=%d nwords=%d", 
					 (int)nsent, hdr.cmd, hdr.len, Res->WWList.nwords);

				  DPS_FREE(wbuf);
				}
#define MAX_PS 1000
				if (last_cmd == DPS_SEARCHD_CMD_WORDS_ALL) {
				  Res->num_rows = Res->total_found;
				  Res->first = 0;
				} else {
				  page_size   = DpsVarListFindInt(&Agent->Vars, "ps", DPS_DEFAULT_PS);
				  page_size   = dps_min(page_size, MAX_PS);
				  page_number = DpsVarListFindInt(&Agent->Vars, "p", 0);
				  if (page_number == 0) {
				    page_number = DpsVarListFindInt(&Agent->Vars, "np", 0);
				  } else page_number--;
				  Res->first = page_number * page_size;	
				  if(Res->first >= Res->total_found) Res->first = (Res->total_found)? (Res->total_found - 1) : 0;
				  if((Res->first + page_size) > Res->total_found){
				    Res->num_rows = Res->total_found - Res->first;
				  }else{
				    Res->num_rows = page_size;
				  }
				}
				Res->last = Res->first + Res->num_rows - 1;
				
				if (Res->PerSite) {
				  hdr.cmd = DPS_SEARCHD_CMD_PERSITE;
				  hdr.len = /*Res->CoordList.ncoords*/ Res->num_rows * sizeof(*Res->PerSite);
				  nsent = DpsSearchdSendPacket(server, &hdr, &Res->PerSite[Res->first]);
				  DpsLog(Agent, verb, "Sent PerSite packet %u bytes cmd=%d len=%d", nsent, hdr.cmd, hdr.len);
				}

				hdr.cmd = DPS_SEARCHD_CMD_DATA;
				hdr.len = /*Res->CoordList.ncoords*/ Res->num_rows * sizeof(*Res->CoordList.Data);
				nsent = DpsSearchdSendPacket(server, &hdr, &Res->CoordList.Data[Res->first]);
				DpsLog(Agent, verb, "Sent URLDATA packet %u bytes cmd=%d len=%d", nsent, hdr.cmd, hdr.len);

#ifdef WITH_REL_TRACK
				hdr.cmd = DPS_SEARCHD_CMD_TRACKDATA;
				hdr.len = /*Res->CoordList.ncoords*/ Res->num_rows * sizeof(*Res->CoordList.Track);
				nsent = DpsSearchdSendPacket(server, &hdr, &Res->CoordList.Track[Res->first]);
				DpsLog(Agent, verb, "Sent TRACKDATA packet %u bytes cmd=%d len=%d", nsent, hdr.cmd, hdr.len);
#endif


#if HAVE_ASPELL
				if (Res->Suggest != NULL) {
				  hdr.cmd = DPS_SEARCHD_CMD_SUGGEST;
				  hdr.len = dps_strlen(Res->Suggest) + 1;
				  nsent = DpsSearchdSendPacket(server, &hdr, Res->Suggest);
				  DpsLog(Agent, verb, "Sent Suggest packet %d bytes cmd=%d len=%d", (int)nsent, hdr.cmd, hdr.len);
				}
#endif

				hdr.cmd=DPS_SEARCHD_CMD_WORDS;
				hdr.len = /*Res->CoordList.ncoords*/ Res->num_rows * sizeof(DPS_URL_CRD_DB);
				nsent=DpsSearchdSendPacket(server, &hdr, &Res->CoordList.Coords[Res->first]);
				DpsLog(Agent,verb,"Sent words packet %d bytes cmd=%d len=%d nwords=%d",(int)nsent,hdr.cmd,hdr.len,Res->CoordList.Coords);

				/*
				for(i=0;i<Agent->total_found;i++){
					dps_snprintf(buf,sizeof(buf)-1,"u=%08X c=%08X",wrd[i].url_id,wrd[i].coord);
					DpsLog(Agent,verb,"%s",buf);
				}
				*/
				break;
			case DPS_SEARCHD_CMD_GOODBYE:
				Res->work_time = DpsStartTimer() - ticks;
				DpsTrackSearchd(Agent, Res);
/*				DpsTrack(Agent, &Agent->Conf->Vars, Res);*/
				DpsLog(Agent,verb,"Received goodbye command. Work time: %.3f sec.", (float)Res->work_time / 1000);
				done=1;
				break;
			default:
				DpsLog(Agent,verb,"Unknown command %d",hdr.cmd);
				done=1;
				break;
		}
	}
	close(server);
	close(client);
	client=0;
	DpsLog(Agent,verb,"Quit");
	DpsResultFree(Res);
	DPS_FREE(words);
	return DPS_OK;
}

/*************************************************************/

static void usage(void){

	fprintf(stderr, "\nsearchd from %s-%s-%s\n(C)1998-2003, LavTech Corp.\n(C)2003-2007, Datapark Corp.\n\n\
Usage: searchd [OPTIONS]\n\
\n\
Options are:\n\
  -l		do not log to stderr\n\
  -v n          verbose level, 0-5\n\
  -s n          sleep n seconds before starting\n\
  -w /path      choose alternative working /var directory\n\
  -f            run foreground, don't demonize\n\
  -h,-?         print this help page and exit\n\
\n\
\n", PACKAGE,VERSION,DPS_DBTYPE);

	return;
}

#define SEARCHD_EXIT    0
#define SEARCHD_RELOAD  1


static int client_main(DPS_ENV *Env, size_t handle) {
  struct timeval tval;
  int		ns;
  struct sockaddr_in client_addr;
  socklen_t	addrlen=sizeof(client_addr);
  char		addr[128]="";
  int		method;
  DPS_MATCH	*M;
  DPS_MATCH_PART	P[10];
  int mdone=0;
  int res=SEARCHD_EXIT;
  DPS_AGENT *Agent = DpsAgentInit(NULL, Env, 300 + (int)handle);

  MaxClients = DpsVarListFindInt(&Env->Vars, "MaxClients", 1);
  if (MaxClients > DPS_CHILDREN_LIMIT) MaxClients = DPS_CHILDREN_LIMIT;

  if (Agent == NULL) {
    DpsLog_noagent(Env, DPS_LOG_ERROR, "Can't alloc Agent at %s:%d", __FILE__, __LINE__);
    exit(1);
  }

  /* (re)setup signals here ?*/
  init_signals();

  while (!mdone) {
    fd_set msk;
    int sel;

    alarm(2400); /* 40 min. - maximum time of child execution */

    DpsVarListFree(&Agent->Vars);
    DpsVarListInit(&Agent->Vars);
    DpsVarListAddLst(&Agent->Vars, &Env->Vars, NULL, "*");
/*
    DpsVarListDel(&Agent->Vars, "q");
    DpsVarListDel(&Agent->Vars, "np");
    DpsVarListDel(&Agent->Vars, "p");
    DpsVarListDel(&Agent->Vars, "E");
    DpsVarListDel(&Agent->Vars, "CS");
    DpsVarListDel(&Agent->Vars, "CP");
    *Env->errstr = '\0';
*/
/*    DpsAcceptMutexLock(Agent);*/

    msk = mask;
    tval.tv_sec = 60;
    tval.tv_usec = 0;
    sel = select(FD_SETSIZE, &msk, 0, 0, &tval);

    if(have_sighup){
      DpsLog(Agent, DPS_LOG_WARN, "SIGHUP arrived");
      have_sighup = 0;
      res = SEARCHD_RELOAD;
      if (parent_pid > 0) kill(parent_pid, SIGHUP);
      mdone = 1;
    }
    if(have_sigint){
      DpsLog(Agent, DPS_LOG_ERROR, "SIGINT arrived");
      have_sigint = 0;
      mdone = 1;
    }
    if(have_sigterm){
      DpsLog(Agent, DPS_LOG_ERROR, "SIGTERM arrived");
      have_sigterm = 0;
      mdone = 1;
    }
    if(have_sigpipe){
      DpsLog(Agent, DPS_LOG_ERROR, "SIGPIPE arrived. Broken pipe!");
      have_sigpipe = 0;
      mdone = 1;
    }
    if (have_sigusr1) {
      DpsIncLogLevel(Agent);
      have_sigusr1 = 0;
    }
    if (have_sigusr2) {
      DpsDecLogLevel(Agent);
      have_sigusr2 = 0;
    }

    if(mdone) {
/*      DpsAcceptMutexUnlock(Agent);*/
      break;
    }

    if(sel == 0) {
/*      DpsAcceptMutexUnlock(Agent);*/
      continue;
    }
    if(sel == -1) {
      switch(errno){
      case EINTR:	/* Child */
	break;
      default:
	DpsLog(Agent, DPS_LOG_WARN, "FIXME select error %d %s", errno, strerror(errno));
      }
/*      DpsAcceptMutexUnlock(Agent);*/
      continue;
    }

    if (FD_ISSET(clt_sock, &msk)) {
      if ((ns = accept(clt_sock, (struct sockaddr *) &client_addr, &addrlen)) == -1) {
	DpsLog(Agent, DPS_LOG_ERROR, "accept() error %d %s", errno, strerror(errno));
	DpsEnvFree(Agent->Conf);
	DpsAgentFree(Agent);
	exit(1);
      }
/*      break;*/
    }
/*    DpsAcceptMutexUnlock(Agent);*/
    DpsLog(Agent, DPS_LOG_EXTRA, "Connect %s", inet_ntoa(client_addr.sin_addr));
    
    dps_snprintf(addr, sizeof(addr)-1, inet_ntoa(client_addr.sin_addr));
    M = DpsMatchListFind(&Agent->Conf->Filters,addr,10,P);
    method = M ? DpsMethod(M->arg) : DPS_METHOD_GET;
    DpsLog(Agent, DPS_LOG_EXTRA, "%s %s %s%s", M?M->arg:"",addr,M?M->pattern:"",M?"":"Allow by default");
			
    if(method == DPS_METHOD_DISALLOW) {
      DpsLog(Agent, DPS_LOG_ERROR, "Reject client");
      close(ns);
      continue;
    }

    do_client(Agent, ns);

    close(ns);

  }
  return res;
}




#define MAXMSG (4096 + sizeof(long))

static void SearchdTrack(DPS_AGENT *Agent) {
  DPS_SQLRES      sqlRes;
  DPS_DBLIST      dbl;
  DPS_DB *db;
  DPS_DBLIST      *DBL;
  DIR * dir;
  struct dirent * item;
  int fd, rec_id, res = DPS_OK;
  ssize_t n;
  char query[MAXMSG];
  char *lt;
  char qbuf[4096 + MAXMSG];
  char fullname[PATH_MAX]="";
  char *IP, *qwords, *qtime, *total_found, *wtime, *var, *val;
  size_t i, dbfrom = 0, dbto; /*=  (Agent->flags & DPS_FLAG_UNOCON) ? Agent->Conf->dbl.nitems : Agent->dbl.nitems;*/ 
  const char      *vardir = DpsVarListFindStr(&Agent->Vars, "VarDir", DPS_VAR_DIR);
  const char      *TrackDBAddr = DpsVarListFindStr(&Agent->Vars, "TrackDBAddr", NULL);
  size_t to_sleep, to_delete;

  init_TrackSignals();

  DpsSQLResInit(&sqlRes);
  if (TrackDBAddr) {
    DBL = &dbl;
    bzero((void*)DBL, sizeof(dbl));
    if(DPS_OK != DpsDBListAdd(DBL, TrackDBAddr, DPS_OPEN_MODE_WRITE)){
      DpsLog(Agent,  DPS_LOG_ERROR, "Invalid TrackDBAddr: '%s'", TrackDBAddr);
      return;
    }
  } else {
    DBL = (Agent->flags & DPS_FLAG_UNOCON) ? &Agent->Conf->dbl : &Agent->dbl;
  }
  dbto = DBL->nitems;

  /* To see the URL being indexed in "ps" output on xBSD */
  dps_setproctitle("Query Tracker");

  for (i = dbfrom; i < dbto; i++) {
    db = &DBL->db[i];
    db->connected = 0;
  }

  while(1) {
    if(have_sigint){
      DpsLog(Agent, DPS_LOG_ERROR, "Query Tracker: SIGINT arrived");
      have_sigint = 0;
      break;
    }
    if(have_sigterm){
      DpsLog(Agent, DPS_LOG_ERROR, "Query Tracker: SIGTERM arrived");
      have_sigterm = 0;
      break;
    }
    if(have_sigpipe){
      DpsLog(Agent, DPS_LOG_ERROR, "Query Tracker: SIGPIPE arrived. Broken pipe !");
      have_sigpipe = 0;
      break;
    }
    if (have_sigusr1) {
      DpsIncLogLevel(Agent);
      have_sigusr1 = 0;
    }
    if (have_sigusr2) {
      DpsDecLogLevel(Agent);
      have_sigusr2 = 0;
    }
    DpsLog(Agent, DPS_LOG_DEBUG, "Query Track: starting directory reading");

    dir = opendir(vardir);
    if (dir) {
      while((item = readdir(dir))) {
	if (strncmp(item->d_name, "track.", 6) && strncmp(item->d_name, "query.", 6)) continue;
	to_delete = 0;
	dps_snprintf(fullname, sizeof(fullname), "%s%s%s", vardir, DPSSLASHSTR, item->d_name);
	if ((fd = DpsOpen2(fullname, O_RDONLY | DPS_BINARY)) > 0) {
	  n = read(fd, query, sizeof(query) - 1);
	  DpsClose(fd);

	  if (n > 0) {
	    query[n] = '\0';


	    DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: query[%d]: %s", dps_strlen(query + sizeof(long)), query + sizeof(long) );
	      
	    to_delete = 1;
	    for (i = dbfrom; (i < dbto); i++) {
	      db = &DBL->db[i];
	      if(TrackDBAddr || db->TrackQuery) {
		const char      *qu = (db->DBType == DPS_DB_PGSQL) ? "'" : "";

		if (strncmp(item->d_name, "track.", 6)) {
		  res = DpsSQLAsyncQuery(db, NULL, query);
		  if (res != DPS_OK) {to_delete = 0; continue; }
		} else {
		  IP = dps_strtok_r(query + sizeof(long), "\2", &lt, NULL);
/*	  DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: IP: %s", IP);*/
		  qwords = dps_strtok_r(NULL, "\2", &lt, NULL);
/*	  DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: qwords: %s", qwords);*/
		  qtime = dps_strtok_r(NULL, "\2", &lt, NULL);
/*	  DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: qtime: %s", qtime);*/
		  total_found = dps_strtok_r(NULL, "\2", &lt, NULL); 
/*	  DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: total: %s", total_found);*/
		  wtime = dps_strtok_r(NULL, "\2", &lt, NULL); 
/*	  DpsLog(Agent, DPS_LOG_EXTRA, "Query Track: wtime: %s", wtime);*/
      
		  dps_snprintf(qbuf, sizeof(qbuf), "INSERT INTO qtrack (ip,qwords,qtime,found,wtime) VALUES ('%s','%s',%s,%s,%s)",
			       IP, qwords, qtime, total_found, wtime );
 
		  res = DpsSQLAsyncQuery(db, NULL, qbuf);
		  if (res != DPS_OK) {to_delete = 0; continue; }

		  dps_snprintf(qbuf, sizeof(qbuf), "SELECT rec_id FROM qtrack WHERE ip='%s' AND qtime=%s", IP, qtime);
		  res = DpsSQLQuery(db, &sqlRes, qbuf);
		  if (res != DPS_OK) { to_delete = 0; continue; }
		  if (DpsSQLNumRows(&sqlRes) == 0) { DpsSQLFree(&sqlRes); res = DPS_ERROR; continue; }
		  rec_id = DPS_ATOI(DpsSQLValue(&sqlRes, 0, 0));
		  DpsSQLFree(&sqlRes);

		  do {
		    var = dps_strtok_r(NULL, "\2", &lt, NULL);
		    if (var != NULL) {
		      val = dps_strtok_r(NULL, "\2", &lt, NULL); 
		      dps_snprintf(qbuf, sizeof(qbuf), "INSERT INTO qinfo (q_id,name,value) VALUES (%s%i%s,'%s','%s')", 
				   qu, rec_id, qu, var, val);
		      res = DpsSQLAsyncQuery(db, NULL, qbuf);
		      if (res != DPS_OK) continue;
		    }
		  } while (var != NULL);
		}
	      }
	    }
	  }

	  if (to_delete) unlink(fullname);
	}
      }
      closedir(dir);
      if (to_delete) {
	to_sleep = 10;
      } else if (to_sleep < 3600) {
	to_sleep += 10;
      }
    } else {
      DpsLog(Agent, DPS_LOG_ERROR, "Can't open directory: %s, [%d:%s]", vardir, errno, strerror(errno));
      if (to_sleep < 3600) {
	to_sleep += 10;
      }
    }

    DPSSLEEP(to_sleep);

  }
}



int main(int argc, char **argv, char **envp) {
        struct sockaddr_in old_server_addr;
	int done = 0, demonize = 1;
	int ch=0, pid_fd;
	int nport=DPS_SEARCHD_PORT;
	const char * config_name = DPS_CONF_DIR "/searchd.conf";
	int debug_level = DPS_LOG_INFO;
	char	pidbuf[64];
	const char	*var_dir = NULL, *pvar_dir = DPS_VAR_DIR;
	pid_t track_pid = 0;
	size_t i;

	log2stderr = 1;
	DpsInit(argc, argv, envp); /* Initialize library */
	DpsInitMutexes();
	parent_pid = getpid();

	while ((ch = getopt(argc, argv, "fhl?v:w:s:")) != -1){
		switch (ch) {
			case 'l':
				log2stderr=0;
				break;
		        case 'v': debug_level = atoi(optarg); break;
			case 'w':
				pvar_dir = optarg;
				break;
		        case 's':
			        sleep((unsigned int)atoi(optarg));
				break;
		        case 'f':
			        demonize = 0;
				break;
			case 'h':
			case '?':
			default:
				usage();
				return 1;
				break;
		}
	}
	argc -= optind;argv += optind;
	if(argc==1)config_name=argv[0];

	dps_snprintf(dps_pid_name, PATH_MAX, "%s%s%s", pvar_dir, DPSSLASHSTR, "searchd.pid");
	pid_fd = DpsOpen3(dps_pid_name, O_CREAT|O_EXCL|O_WRONLY, 0644);
	if(pid_fd < 0) {
		fprintf(stderr, "Can't create '%s': %s\n", dps_pid_name, strerror(errno));
		if(errno == EEXIST){
		  fprintf(stderr, "It seems that another searchd is already running!\nRemove '%s' if it is not true.", dps_pid_name);
		}
		return DPS_ERROR;
	}

	if (demonize) {
	  if (dps_demonize() != 0) {
	    fprintf(stderr, "Can't demonize\n");
	    DpsClose(pid_fd);
	    exit(1);
	  }
	}
	sprintf(pidbuf, "%d\n", (int)getpid());
	write(pid_fd, &pidbuf, dps_strlen(pidbuf));
	DpsClose(pid_fd);

	bzero(Children, DPS_CHILDREN_LIMIT * sizeof(DPS_CHILD));
	bzero(&old_server_addr, sizeof(old_server_addr));

	while(!done){
		struct sockaddr_in server_addr;
		int on = 1;
		DPS_AGENT * Agent;
		DPS_ENV * Conf=NULL;
		int verb=-1;
		dps_uint8 flags = DPS_FLAG_SPELL | DPS_FLAG_UNOCON | DPS_FLAG_ADD_SERV;
		int res = 0, internaldone = 0;
		const char *lstn;

		if (track_pid != 0) {
		  kill(track_pid, SIGTERM);
		}

		Conf = DpsEnvInit(NULL);
		if (Conf == NULL) {
		  fprintf(stderr, "Can't alloc Env at %s:%d", __FILE__, __LINE__);
		  return DPS_ERROR;
		}
		Agent=DpsAgentInit(NULL,Conf,0);
		
		if (Agent == NULL) {
		  fprintf(stderr, "Can't alloc Agent at %s:%d", __FILE__, __LINE__);
		  return DPS_ERROR;
		}

		Agent->flags = Conf->flags = flags; /*DPS_FLAG_UNOCON | DPS_FLAG_ADD_SERV;*/

		res = DpsEnvLoad(Agent, config_name, flags);
		
		if (pvar_dir == NULL) var_dir = DpsVarListFindStr(&Conf->Vars, "VarDir", DPS_VAR_DIR);
		else var_dir = pvar_dir;

/*		DpsAcceptMutexInit(var_dir);*/
		DpsUniRegCompileAll(Conf);

		DpsOpenLog("searchd", Conf, log2stderr);
		
		DpsSetLogLevel(Agent, debug_level);
		Agent->flags = Conf->flags = flags;
		Agent->Flags = Conf->Flags;

		if(res!=DPS_OK){
			DpsLog(Agent,verb,"%s",DpsEnvErrMsg(Conf));
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			exit(1);
		}

		if (Conf->Flags.PreloadURLData) {
		  DpsLog(Agent, verb, "Preloading url data");
		  DpsURLDataPreload(Agent);
		}
		MaxClients = DpsVarListFindInt(&Agent->Conf->Vars, "MaxClients", 1);
		if (MaxClients > DPS_CHILDREN_LIMIT) MaxClients = DPS_CHILDREN_LIMIT;
		DpsLog(Agent, verb, "searchd started with '%s'", config_name);
		DpsLog(Agent, verb, "VarDir: '%s'", DpsVarListFindStr(&Agent->Conf->Vars, "VarDir", DPS_VAR_DIR));
		DpsLog(Agent, verb, "MaxClients: %d\n", MaxClients);
		DpsLog(Agent, verb, "Affixes: %d, Spells: %d, Synonyms: %d, Acronyms: %d, Stopwords: %d",
		       Conf->Affixes.naffixes,Conf->Spells.nspell,
		       Conf->Synonyms.nsynonyms,
		       Conf->Acronyms.nacronyms,
		       Conf->StopWords.nstopwords);
		DpsLog(Agent,verb,"Chinese dictionary with %d entries", Conf->Chi.nwords);
		DpsLog(Agent,verb,"Korean dictionary with %d entries", Conf->Korean.nwords);
		DpsLog(Agent,verb,"Thai dictionary with %d entries", Conf->Thai.nwords);

		if ((track_pid = fork() ) == -1) {
		  DpsLog(Agent, DPS_LOG_ERROR, "fork() error %d %s", errno, strerror(errno));
		  DpsAgentFree(Agent);
		  DpsEnvFree(Conf);
		  unlink(dps_pid_name);
		  exit(1);
		}
		if (track_pid == 0) { /* child process */
		  DpsVarListAddLst(&Agent->Vars, &Agent->Conf->Vars, NULL, "*");
		  SearchdTrack(Agent);
		  DpsAgentFree(Agent);
		  DpsEnvFree(Conf);
		  exit(0);
		}
		DpsLog(Agent,verb,"Query tracker child started.");

#if defined(HAVE_PTHREAD) && defined(HAVE_THR_SETCONCURRENCY_PROT)
		if (thr_setconcurrency(16) != NULL) { /* FIXME: Does 16 is enough ? */
		  DpsLog(A, DPS_LOG_ERROR, "Can't set %d concurrency threads", maxthreads);
		  return DPS_ERROR;
		}
#endif
		
		bzero((void*)&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		if((lstn=DpsVarListFindStr(&Agent->Conf->Vars,"Listen",NULL))){
			char * cport;
			
			if((cport=strchr(lstn, ':'))) {
				*cport = '\0';
				server_addr.sin_addr.s_addr = inet_addr(lstn);
				nport=atoi(cport+1);
			}else if (strchr(lstn, '.')) {
			        server_addr.sin_addr.s_addr = inet_addr(lstn);
			}else{
				nport = atoi(lstn);
				server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			}
			DpsLog(Agent, verb, "Listening port %d", nport);
		}else{
			DpsLog(Agent,verb,"Listening port %d",nport);
			server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		server_addr.sin_port = htons((u_short)nport);

		if (memcmp(&old_server_addr, &server_addr, sizeof(server_addr)) != 0) {

		  FD_ZERO(&mask);
		
		  if (clt_sock > 0) dps_closesocket(clt_sock);

		  if ((clt_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			DpsLog(Agent, DPS_LOG_ERROR, "socket() error %d", errno);
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			exit(1);
		  }
		  
		  if (setsockopt(clt_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) != 0){
			DpsLog(Agent, DPS_LOG_ERROR, "setsockopt() error %d", errno);
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			exit(1);
		  }
		  DpsSockOpt(Agent, clt_sock);


		  if (bind(clt_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
			DpsLog(Agent, DPS_LOG_ERROR, "Can't bind: error %d %s", errno, strerror(errno));
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			exit(1);
		  }

		  /* Backlog 64 is enough? */
		  if (listen(clt_sock, 64) == -1) {
			DpsLog(Agent, DPS_LOG_ERROR, "listen() error %d %s", errno, strerror(errno));
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			exit(1);
		  }
		  FD_SET(clt_sock, &mask);
		  dps_memcpy(&old_server_addr, &server_addr, sizeof(server_addr));
		}
		
		
		DpsLog(Agent, DPS_LOG_WARN, "Ready");
		
		init_signals();

		for (i = 0; i < DPS_CHILDREN_LIMIT; i++) {
		  if (Children[i].pid) kill(Children[i].pid, SIGTERM);
		  Children[i].pid = 0;
		}

		if(DpsSearchCacheClean(Agent) != DPS_OK) {
			DpsLog(Agent, DPS_LOG_ERROR, "Error in search cache cleaning");
			DpsAgentFree(Agent);
			DpsEnvFree(Conf);
			unlink(dps_pid_name);
			DpsDestroyMutexes();
			exit(1);
		}

		for (i = 0; i < MaxClients; i++) {
		  pid_t pid;
/*		  if (Children[i].pid) kill(Children[i].pid, SIGTERM);*/
		  pid = fork();
		  if(pid == 0){
		    client_main(Agent->Conf, i);
		    DpsEnvFree(Agent->Conf);
		    DpsAgentFree(Agent);
		    exit(0);
		  } else {
		    if (pid > 0) {
		      Children[i].pid = pid;
		    } else {
		      Children[i].pid = 0;
		      DpsLog(Agent, DPS_LOG_ERROR, "fork() error %d", pid);
		    }
		  }
		}

		while(!internaldone) {
		  DPSSLEEP(1);
		  if(have_sighup){
			DpsLog(Agent,verb,"SIGHUP arrived");
			have_sighup=0;
			res = SEARCHD_RELOAD;
			internaldone = 1;
		  }
		  if(have_sigint){
			DpsLog(Agent,verb,"SIGINT arrived");
			have_sigint=0;
			res = SEARCHD_EXIT;
			internaldone = 1;
		  }
		  if(have_sigterm){
			DpsLog(Agent,verb,"SIGTERM arrived");
			have_sigterm=0;
			res = SEARCHD_EXIT;
			internaldone = 1;
		  }
		  if(have_sigpipe){ /* FIXME: why this may happens and what to do with it */
			DpsLog(Agent, verb, "SIGPIPE arrived. Broken pipe!");
			have_sigpipe = 0;
/*			mdone = 1;*/
		  }
		  if (have_sigusr1) {
		    DpsIncLogLevel(Agent);
		    have_sigusr1 = 0;
		  }
		  if (have_sigusr2) {
		    DpsDecLogLevel(Agent);
		    have_sigusr2 = 0;
		  }
		  for (i = 0; i < MaxClients; i++) {
		    if (Children[i].pid == 0) {
		      pid_t pid = fork();
		      if(pid == 0){
			client_main(Agent->Conf, i);
			DpsEnvFree(Agent->Conf);
			DpsAgentFree(Agent);
			exit(0);
		      } else {
			if (pid > 0) {
			  Children[i].pid = pid;
			} else {
			  Children[i].pid = 0;
			  DpsLog(Agent, DPS_LOG_ERROR, "fork() error %d", pid);
			}
		      }
		    }
		  }

		}

		if(res != SEARCHD_RELOAD){
			done = 1;
			DpsLog(Agent,verb,"Shutdown");
		}else{
			DpsLog(Agent,verb,"Reloading conf");
		}

		if (Conf->Flags.PreloadURLData) {
		  DpsURLDataDePreload(Agent);
		}
		DpsAgentFree(Agent);
		DpsEnvFree(Conf);
	}
		
	for (i = 0; i < MaxClients; i++) {
	  if (Children[i].pid) kill(Children[i].pid, SIGTERM);
	}
	if (track_pid != 0) {
	  kill(track_pid, SIGTERM);
	}

	unlink(dps_pid_name);
	DpsDestroyMutexes();

#ifdef EFENCE
	fprintf(stderr, "Memory leaks checking\n");
	DpsEfenceCheckLeaks();
#endif
#ifdef FILENCE
	fprintf(stderr, "FD leaks checking\n");
	DpsFilenceCheckLeaks(NULL);
#endif

#ifdef BOEHMGC
	CHECK_LEAKS();
#endif
     
	return DPS_OK;
}
