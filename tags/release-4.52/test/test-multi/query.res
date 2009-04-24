SQL>'FIELDS=OFF'
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32'
200	198	1	-1681908011	http://site/test4.html
200	817	1	-1316097023	http://site/test1.html
200	122	1	-246447934	http://site/test3.html
200	165	0	0	http://site/
200	164	1	1500723064	http://site/test2.html
200	69	1	1874677200	http://site/test1.txt
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	-1681908011	http://site/test4.html	body	This is the 4th test page. Here is the third one. проверка виндовой кодироки общественные
200	-1681908011	http://site/test4.html	Charset	windows-1251
200	-1681908011	http://site/test4.html	Content-Language	ru
200	-1681908011	http://site/test4.html	Content-Type	text/html
200	-1681908011	http://site/test4.html	title	Test 4 title
200	-1316097023	http://site/test1.html	body	This is the first test page. Here is the second one. 1 22 333 4444 55555 666666 7777777 88888888 999999999 AAAAAAAAAA BBBBBBBBBBB CCCCCCCCCCCC DDDDDDDDDDDDD EEEEEEEEEEEEEE FFFFFFFFFFFFFFF GGGGGGGGGGGGGGGG HHHHHHHHHHHHHHHHH IIIIIIIIIIIIIIIIII JJJJJJJJJJJJJJJ
200	-1316097023	http://site/test1.html	Charset	ISO-8859-1
200	-1316097023	http://site/test1.html	Content-Language	en
200	-1316097023	http://site/test1.html	Content-Type	text/html
200	-1316097023	http://site/test1.html	title	Test 1 title
200	-246447934	http://site/test3.html	body	This is the third test. No more tests available.
200	-246447934	http://site/test3.html	Charset	ISO-8859-1
200	-246447934	http://site/test3.html	Content-Language	en
200	-246447934	http://site/test3.html	Content-Type	text/html
200	-246447934	http://site/test3.html	title	Test 3 title
200	0	http://site/	Charset	KOI8-R
200	0	http://site/	Content-Language	ru
200	0	http://site/	Content-type	text/html
200	1500723064	http://site/test2.html	body	This is the second test page. Here is the third one. ÏÂÝÅÓÔ×ÅÎÎÙÅ
200	1500723064	http://site/test2.html	Charset	ISO-8859-1
200	1500723064	http://site/test2.html	Content-Language	en
200	1500723064	http://site/test2.html	Content-Type	text/html
200	1500723064	http://site/test2.html	title	Test 2 title
200	1874677200	http://site/test1.txt	body	This is a text file. No title available. abcdefghijklmnopqrstuvwxyz
200	1874677200	http://site/test1.txt	Charset	ISO-8859-1
200	1874677200	http://site/test1.txt	Content-Language	en
200	1874677200	http://site/test1.txt	Content-Type	text/plain
SQL>SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict3  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
the	393472	-1681908011	http://site/test4.html
4th	459008	-1681908011	http://site/test4.html
the	786688	-1681908011	http://site/test4.html
one	917760	-1681908011	http://site/test4.html
the	393472	-1316097023	http://site/test1.html
the	786688	-1316097023	http://site/test1.html
one	917760	-1316097023	http://site/test1.html
333	1114368	-1316097023	http://site/test1.html
the	393472	-246447934	http://site/test3.html
the	393472	1500723064	http://site/test2.html
the	786688	1500723064	http://site/test2.html
one	917760	1500723064	http://site/test2.html
txt	853504	1874677200	http://site/test1.txt
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict4  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
test	66048	-1681908011	http://site/test4.html
this	262400	-1681908011	http://site/test4.html
test	524544	-1681908011	http://site/test4.html
page	590080	-1681908011	http://site/test4.html
here	655616	-1681908011	http://site/test4.html
http	1247488	-1681908011	http://site/test4.html
site	1312768	-1681908011	http://site/test4.html
html	1443328	-1681908011	http://site/test4.html
test	66048	-1316097023	http://site/test1.html
this	262400	-1316097023	http://site/test1.html
test	524544	-1316097023	http://site/test1.html
page	590080	-1316097023	http://site/test1.html
here	655616	-1316097023	http://site/test1.html
4444	1179904	-1316097023	http://site/test1.html
http	3082496	-1316097023	http://site/test1.html
site	3147776	-1316097023	http://site/test1.html
html	3278336	-1316097023	http://site/test1.html
test	66048	-246447934	http://site/test3.html
this	262400	-246447934	http://site/test3.html
test	524544	-246447934	http://site/test3.html
more	655616	-246447934	http://site/test3.html
http	854272	-246447934	http://site/test3.html
site	919552	-246447934	http://site/test3.html
html	1050112	-246447934	http://site/test3.html
http	67840	0	http://site/
site	133120	0	http://site/
test	66048	1500723064	http://site/test2.html
this	262400	1500723064	http://site/test2.html
test	524544	1500723064	http://site/test2.html
page	590080	1500723064	http://site/test2.html
here	655616	1500723064	http://site/test2.html
http	1050880	1500723064	http://site/test2.html
site	1116160	1500723064	http://site/test2.html
html	1246720	1500723064	http://site/test2.html
this	65792	1874677200	http://site/test1.txt
text	262400	1874677200	http://site/test1.txt
file	327936	1874677200	http://site/test1.txt
http	657664	1874677200	http://site/test1.txt
site	722944	1874677200	http://site/test1.txt
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict5  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
title	197120	-1681908011	http://site/test4.html
third	852224	-1681908011	http://site/test4.html
test4	1377792	-1681908011	http://site/test4.html
title	197120	-1316097023	http://site/test1.html
first	459008	-1316097023	http://site/test1.html
55555	1245440	-1316097023	http://site/test1.html
test1	3212800	-1316097023	http://site/test1.html
title	197120	-246447934	http://site/test3.html
third	459008	-246447934	http://site/test3.html
tests	721152	-246447934	http://site/test3.html
test3	984576	-246447934	http://site/test3.html
title	197120	1500723064	http://site/test2.html
third	852224	1500723064	http://site/test2.html
test2	1181184	1500723064	http://site/test2.html
title	459008	1874677200	http://site/test1.txt
test1	787968	1874677200	http://site/test1.txt
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict6  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
second	852224	-1316097023	http://site/test1.html
666666	1310976	-1316097023	http://site/test1.html
second	459008	1500723064	http://site/test2.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict7  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
7777777	1376512	-1316097023	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict8  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
88888888	1442048	-1316097023	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict9  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
999999999	1507584	-1316097023	http://site/test1.html
available	786688	-246447934	http://site/test3.html
available	524544	1874677200	http://site/test1.txt
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict10 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
aaaaaaaaaa	1573120	-1316097023	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict11 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
bbbbbbbbbbb	1638656	-1316097023	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict12 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
cccccccccccc	1704192	-1316097023	http://site/test1.html
SQL>SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict32 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
общественные	1179904	-1681908011	http://site/test4.html
hhhhhhhhhhhhhhhhh	2031872	-1316097023	http://site/test1.html
iiiiiiiiiiiiiiiiii	2097408	-1316097023	http://site/test1.html
jjjjjjjjjjjjjjjjjjj	2162944	-1316097023	http://site/test1.html
kkkkkkkkkkkkkkkkkkkk	2228480	-1316097023	http://site/test1.html
lllllllllllllllllllll	2294016	-1316097023	http://site/test1.html
mmmmmmmmmmmmmmmmmmmmmm	2359552	-1316097023	http://site/test1.html
nnnnnnnnnnnnnnnnnnnnnnn	2425088	-1316097023	http://site/test1.html
oooooooooooooooooooooooo	2490624	-1316097023	http://site/test1.html
ppppppppppppppppppppppppp	2556160	-1316097023	http://site/test1.html
qqqqqqqqqqqqqqqqqqqqqqqqqq	2621696	-1316097023	http://site/test1.html
rrrrrrrrrrrrrrrrrrrrrrrrrrr	2687232	-1316097023	http://site/test1.html
ssssssssssssssssssssssssssss	2752768	-1316097023	http://site/test1.html
ttttttttttttttttttttttttttttt	2818304	-1316097023	http://site/test1.html
uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu	2883840	-1316097023	http://site/test1.html
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv	2949376	-1316097023	http://site/test1.html
wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww	3014912	-1316097023	http://site/test1.html
ïâýåóô×åîîùå	983296	1500723064	http://site/test2.html
abcdefghijklmnopqrstuvwxyz	590080	1874677200	http://site/test1.txt
SQL>
