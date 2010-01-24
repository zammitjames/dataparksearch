SQL>'FIELDS=OFF'
SQL>'SELECT dict.word,dict.intag,url.crc32,url.url FROM dict, url WHERE url.rec_id=dict.url_id ORDER BY url.crc32,dict.intag'
s2	66050	265452973	http://site2/test.html
t2	131586	265452973	http://site2/test.html
ss2	196867	265452973	http://site2/test.html
b2	262402	265452973	http://site2/test.html
site1	327941	265452973	http://site2/test.html
s1	66050	1933740721	http://site1/test.html
t1	131586	1933740721	http://site1/test.html
ss1	196867	1933740721	http://site1/test.html
bb1	262403	1933740721	http://site1/test.html
site2	327941	1933740721	http://site1/test.html
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32, hops'
200	89	0	0	http://site1/
200	89	2	0	http://site2/
200	109	3	265452973	http://site2/test.html
200	110	1	1933740721	http://site1/test.html
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	0	http://site1/	Charset	ISO-8859-1
200	0	http://site2/	Charset	ISO-8859-1
200	0	http://site1/	Content-Language	en
200	0	http://site2/	Content-Language	en
200	0	http://site2/	Content-type	text/html
200	0	http://site1/	Content-type	text/html
200	265452973	http://site2/test.html	body	Ss2 b2. Site1.
200	265452973	http://site2/test.html	Charset	ISO-8859-1
200	265452973	http://site2/test.html	Content-Language	en
200	265452973	http://site2/test.html	Content-Type	text/html
200	265452973	http://site2/test.html	title	S2 t2
200	1933740721	http://site1/test.html	body	Ss1 bb1. Site2.
200	1933740721	http://site1/test.html	Charset	ISO-8859-1
200	1933740721	http://site1/test.html	Content-Language	en
200	1933740721	http://site1/test.html	Content-Type	text/html
200	1933740721	http://site1/test.html	title	S1 t1
SQL>SQL>
