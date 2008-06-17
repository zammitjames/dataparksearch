SQL>'FIELDS=OFF'
SQL>'SELECT dict.word,dict.intag,url.crc32,url.url FROM dict, url WHERE url.rec_id=dict.url_id ORDER BY url.crc32,dict.intag'
report	65792	1471521147	http://site/test.html
bugs	131328	1471521147	http://site/test.html
here	196864	1471521147	http://site/test.html
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32'
200	77	0	0	http://site/
200	92	1	1471521147	http://site/test.html
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	0	http://site/	Content-type	text/html
200	1471521147	http://site/test.html	body	Report bugs here
200	1471521147	http://site/test.html	Content-Type	text/html
SQL>SQL>
