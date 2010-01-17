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
#include "dps_socket.h"
#include "dps_host.h"
#include "dps_utils.h"
#include "dps_xmalloc.h"
#include "dps_http.h"
#include "dps_conf.h"
#include "dps_contentencoding.h"
#include "dps_url.h"
#include "dps_vars.h"
#include "dps_hrefs.h"
#include "dps_textlist.h"
#include "dps_cookies.h"
#include "dps_charsetutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

static void DpsParseHTTPHeader(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc, DPS_DSTR *header) {
  char *val, *header_name;
  char	secname[128], savec;
  DPS_VAR	*Sec;
  DPS_TEXTITEM Item;

  if ((val = strchr(header_name = header->data, ':'))) {
/*
  fprintf(stderr, "HEADER: %s\n", header_name);
*/
    *val++='\0';
    val = DpsTrim(val," \t:");
			
    if (!strcasecmp(header_name, "Content-Type") || !strcasecmp(header_name, "Content-Encoding")) {
      char *v;
      for(v=val ; *v ; v++) 
	*v = dps_tolower(*v);
    } else if (Doc->Spider.use_cookies && !strcasecmp(header_name, "Set-Cookie")) {
      char *part, *lpart;
      char *name = NULL;
      char *value = NULL;
      const char *domain = NULL;
      const char *path = NULL;
      dps_uint4 expire = 0;
      char secure = 'n';
      for (part = dps_strtok_r(val, ";" , &lpart, &savec) ; part;
	   part = dps_strtok_r(NULL, ";", &lpart, &savec)) {
	char *arg;
	part = DpsTrim(part, " ");
	if ((arg = strchr(part, '='))) {
	  *arg++ = '\0';
	  if (!name) {
	    name = part;
	    value = arg;
	  } else 
	    if (!strcasecmp(part, "path")) {
	      path = arg;
	    } else
	      if (!strcasecmp(part, "domain")) {
		domain = arg;
	      } else
		if (!strcasecmp(part, "secure")) {
		  secure = 'y';
		} else
		  if (!strcasecmp(part, "expires")) {
		    expire = (dps_uint4)DpsHttpDate2Time_t(arg);
		  }
	}
      }
      if (name && value) {
	if (domain && domain[0] == '.') {
	  domain++;
	} else {
	  domain = Doc->CurURL.hostname ? Doc->CurURL.hostname : "localhost";
	}
	if (!path) {
	  path = Doc->CurURL.path ? Doc->CurURL.path : "/";
	}
	DpsCookiesAdd(Indexer, domain, path, name, value, secure, expire, 1);
      }
/*			  token = dps_strtok_r(NULL,"\r\n",&lt);
			  continue;*/
      return;
    }
  }

  DpsVarListReplaceStr(&Doc->Sections, header_name, val ? val : "<NULL>");

  dps_snprintf(secname,sizeof(secname),"header.%s", header_name);
  secname[sizeof(secname)-1]='\0';
  if((Sec = DpsVarListFind(&Doc->Sections, secname)) && val ) {
    Item.href = NULL;
    Item.str = val;
    Item.section = Sec->section;
    Item.section_name = secname;
    Item.len = 0;
    DpsTextListAdd(&Doc->TextList, &Item);
  }
}

void DpsParseHTTPResponse(DPS_AGENT *Indexer, DPS_DOCUMENT *Doc) {			
  char	*token, *lt, *headers, savec;
	int     oldstatus;
	DPS_DSTR header;
	
	Doc->Buf.content=NULL;
	oldstatus = DpsVarListFindInt(&Doc->Sections, "Status", 0);
	DpsVarListReplaceInt(&Doc->Sections, "ResponseSize", (int)Doc->Buf.size);
	DpsVarListDel(&Doc->Sections, "Content-Length");
	DpsVarListDel(&Doc->Sections, "Last-Modified");

	if (Doc->Buf.buf == NULL) return;

	/* Cut HTTP response header first        */
	for(token=Doc->Buf.buf;*token;token++){
	  if(!strncmp(token,"\r\n\r\n",4)){
	    if (token <= Doc->Buf.buf + Doc->Buf.size - 4) {
			*token='\0';
			Doc->Buf.content = token + 4;
	    }
	    break;
	  } else if(!strncmp(token,"\n\n",2)){
	    if (token <= Doc->Buf.buf + Doc->Buf.size - 2) {
			*token='\0';
			Doc->Buf.content = token + 2;
	    }
	    break;
	  }
	}
	
	/* Bad response, return */
	if(!Doc->Buf.content) {
	  if (token <= Doc->Buf.buf + Doc->Buf.size - 4) {
	    if (token[2] == CR_CHAR) Doc->Buf.content = token + 4;
	    else Doc->Buf.content = token + 2;
	  }
	}
	
	/* Copy headers not to break them */
	headers = (char*)DpsStrdup(Doc->Buf.buf);
	
	/* Now lets parse response header lines */
	token = dps_strtok_r(headers, "\r\n", &lt, &savec);
	
	if(!token)return;
	
	if(!strncmp(token,"HTTP/",5)){
		int	status = atoi(token + 8);
		DpsVarListReplaceStr(&Doc->Sections,"ResponseLine",token);
		DpsVarListReplaceInt(&Doc->Sections, "Status", (oldstatus > status) ? oldstatus : status );
	}else{
	        DpsFree(headers);
		return;
	}
	token = dps_strtok_r(NULL, "\r\n", &lt, &savec);
	DpsDSTRInit(&header, 128);
	
	while(token){
	
		if(strchr(token,':')) {

		  if (header.data_size) {
		    DpsParseHTTPHeader(Indexer, Doc, &header);
		    DpsDSTRFree(&header);
		    DpsDSTRInit(&header, 128);
		  }

		}
		DpsDSTRAppendStr(&header, token);

		token = dps_strtok_r(NULL, "\r\n", &lt, &savec);
	}
	if (header.data_size) {
	  DpsParseHTTPHeader(Indexer, Doc, &header);
	}
	DpsDSTRFree(&header);
	DPS_FREE(headers);
	
	/* Bad response, return */
	if(!Doc->Buf.content) {
	    return;
	}
	DpsVarListReplaceInt(&Doc->Sections,"Content-Length", Doc->Buf.buf-Doc->Buf.content+(int)Doc->Buf.size + DpsVarListFindInt(&Doc->Sections,"Content-Length", 0));
}
