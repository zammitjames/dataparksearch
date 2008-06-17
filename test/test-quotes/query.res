SQL>'FIELDS=OFF'
SQL>'SELECT dict.word,dict.intag,url.crc32,url.url FROM dict, url WHERE url.rec_id=dict.url_id ORDER BY url.crc32,dict.intag'
this	65792	1077021869	http://site/test1.txt
is	131328	1077021869	http://site/test1.txt
a	196864	1077021869	http://site/test1.txt
text	262400	1077021869	http://site/test1.txt
file	327936	1077021869	http://site/test1.txt
no	393472	1077021869	http://site/test1.txt
title	459008	1077021869	http://site/test1.txt
available	524544	1077021869	http://site/test1.txt
some	590080	1077021869	http://site/test1.txt
special	655616	1077021869	http://site/test1.txt
characters	721152	1077021869	http://site/test1.txt
let's	786688	1077021869	http://site/test1.txt
check	852224	1077021869	http://site/test1.txt
how	917760	1077021869	http://site/test1.txt
they	983296	1077021869	http://site/test1.txt
get	1048832	1077021869	http://site/test1.txt
escaped	1114368	1077021869	http://site/test1.txt
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32'
200	77	0	0	http://site/
200	149	1	1077021869	http://site/test1.txt
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	0	http://site/	Content-type	text/html
200	1077021869	http://site/test1.txt	body	This is a text file. No title available. Some special characters: &#34; '. ' . ' . ' . ' &#34; . &#34; . &#34; . &#34; \ \ \ \ \ Let's check how they get escaped.
200	1077021869	http://site/test1.txt	Content-Type	text/plain
SQL>SQL>
