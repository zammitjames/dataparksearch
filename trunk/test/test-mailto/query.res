SQL>'FIELDS=OFF'
SQL>'SELECT dict.word,dict.intag,url.crc32,url.url FROM dict, url WHERE url.rec_id=dict.url_id ORDER BY url.crc32,dict.intag'
report	65798	-1721616906	http://site/test.html
bugs	131332	-1721616906	http://site/test.html
here	196868	-1721616906	http://site/test.html
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32'
200	92	1	-1721616906	http://site/test.html
200	89	0	0	http://site/
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	-1721616906	http://site/test.html	body	Report bugs here
200	-1721616906	http://site/test.html	Charset	ISO-8859-1
200	-1721616906	http://site/test.html	Content-Language	en
200	-1721616906	http://site/test.html	Content-Type	text/html
200	0	http://site/	Charset	ISO-8859-1
200	0	http://site/	Content-Language	en
200	0	http://site/	Content-type	text/html
SQL>SQL>
