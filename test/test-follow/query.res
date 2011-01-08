SQL>'FIELDS=OFF'
SQL>'SELECT dict.word,dict.intag,url.crc32,url.url FROM dict, url WHERE url.rec_id=dict.url_id ORDER BY url.crc32,dict.intag'
s1	66050	-683249021	http://site1/test.html
t1	131586	-683249021	http://site1/test.html
ss1	196867	-683249021	http://site1/test.html
bb1	262403	-683249021	http://site1/test.html
site2	327941	-683249021	http://site1/test.html
an	393474	-683249021	http://site1/test.html
ok	459010	-683249021	http://site1/test.html
link	524548	-683249021	http://site1/test.html
a	590081	-683249021	http://site1/test.html
stange	655622	-683249021	http://site1/test.html
link	721156	-683249021	http://site1/test.html
http	66052	198092925	http://site1/
site1	131589	198092925	http://site1/
test	196868	198092925	http://site1/
html	262404	198092925	http://site1/
s2	66050	265452973	http://site2/test.html
t2	131586	265452973	http://site2/test.html
ss2	196867	265452973	http://site2/test.html
b2	262402	265452973	http://site2/test.html
site1	327941	265452973	http://site2/test.html
http	66052	947361423	http://site2/
site2	131589	947361423	http://site2/
test	196868	947361423	http://site2/
html	262404	947361423	http://site2/
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32, hops'
200	216	1	-683249021	http://site1/test.html
200	129	0	198092925	http://site1/
200	109	3	265452973	http://site2/test.html
200	129	2	947361423	http://site2/
404	0	2	0	http://site2/?param
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	-683249021	http://site1/test.html	body	Ss1 bb1. Site2. an OK link. a stange link.
200	-683249021	http://site1/test.html	Charset	ISO-8859-1
200	-683249021	http://site1/test.html	Content-Language	en
200	-683249021	http://site1/test.html	Content-Type	text/html
200	-683249021	http://site1/test.html	title	S1 t1
200	198092925	http://site1/	body	.. test.html
200	198092925	http://site1/	Charset	ISO-8859-1
200	198092925	http://site1/	Content-Language	en
200	198092925	http://site1/	Content-type	text/html
200	198092925	http://site1/	title	http://site1/
200	265452973	http://site2/test.html	body	Ss2 b2. Site1.
200	265452973	http://site2/test.html	Charset	ISO-8859-1
200	265452973	http://site2/test.html	Content-Language	en
200	265452973	http://site2/test.html	Content-Type	text/html
200	265452973	http://site2/test.html	title	S2 t2
200	947361423	http://site2/	body	.. test.html
200	947361423	http://site2/	Charset	ISO-8859-1
200	947361423	http://site2/	Content-Language	en
200	947361423	http://site2/	Content-type	text/html
200	947361423	http://site2/	title	http://site2/
SQL>SQL>
