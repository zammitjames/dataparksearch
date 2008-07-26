SQL>'FIELDS=OFF'
SQL>'SELECT status, docsize, hops, crc32, url FROM url ORDER BY status, crc32'
200	164	1	-1740442857	http://site/test2.html
200	69	1	-261021753	http://site/test1.txt
200	165	0	0	http://site/
200	817	1	108051514	http://site/test1.html
200	122	1	1305968173	http://site/test3.html
200	198	1	1951259329	http://site/test4.html
SQL>'SELECT url.status,url.crc32,url.url,urlinfo.sname,urlinfo.sval FROM url,urlinfo WHERE url.rec_id=urlinfo.url_id ORDER BY url.status,url.crc32,lower(urlinfo.sname)'
200	-1740442857	http://site/test2.html	body	This is the second test page. Here is the third one. ÏÂÝÅÓÔ×ÅÎÎÙÅ
200	-1740442857	http://site/test2.html	Charset	ISO-8859-1
200	-1740442857	http://site/test2.html	Content-Language	en
200	-1740442857	http://site/test2.html	Content-Type	text/html
200	-1740442857	http://site/test2.html	title	Test 2 title
200	-261021753	http://site/test1.txt	body	This is a text file. No title available. abcdefghijklmnopqrstuvwxyz
200	-261021753	http://site/test1.txt	Charset	ISO-8859-1
200	-261021753	http://site/test1.txt	Content-Language	en
200	-261021753	http://site/test1.txt	Content-Type	text/plain
200	0	http://site/	Charset	KOI8-R
200	0	http://site/	Content-Language	ru
200	0	http://site/	Content-type	text/html
200	108051514	http://site/test1.html	body	This is the first test page. Here is the second one. 1 22 333 4444 55555 666666 7777777 88888888 999999999 AAAAAAAAAA BBBBBBBBBBB CCCCCCCCCCCC DDDDDDDDDDDDD EEEEEEEEEEEEEE FFFFFFFFFFFFFFF GGGGGGGGGGGGGGGG HHHHHHHHHHHHHHHHH IIIIIIIIIIIIIIIIII JJJJJJJJJJJJJJ
200	108051514	http://site/test1.html	Charset	ISO-8859-1
200	108051514	http://site/test1.html	Content-Language	en
200	108051514	http://site/test1.html	Content-Type	text/html
200	108051514	http://site/test1.html	title	Test 1 title
200	1305968173	http://site/test3.html	body	This is the third test. No more tests available.
200	1305968173	http://site/test3.html	Charset	ISO-8859-1
200	1305968173	http://site/test3.html	Content-Language	en
200	1305968173	http://site/test3.html	Content-Type	text/html
200	1305968173	http://site/test3.html	title	Test 3 title
200	1951259329	http://site/test4.html	body	This is the 4th test page. Here is the third one. ïðîâåðêà âèíäîâîé êîäèðîêè îáùåñòâåííûå
200	1951259329	http://site/test4.html	Charset	ISO-8859-1
200	1951259329	http://site/test4.html	Content-Language	en
200	1951259329	http://site/test4.html	Content-Type	text/html
200	1951259329	http://site/test4.html	title	Test 4 title
SQL>SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict3  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
the	393472	-1740442857	http://site/test2.html
the	786688	-1740442857	http://site/test2.html
one	917760	-1740442857	http://site/test2.html
txt	853504	-261021753	http://site/test1.txt
the	393472	108051514	http://site/test1.html
the	786688	108051514	http://site/test1.html
one	917760	108051514	http://site/test1.html
333	1114368	108051514	http://site/test1.html
the	393472	1305968173	http://site/test3.html
the	393472	1951259329	http://site/test4.html
4th	459008	1951259329	http://site/test4.html
the	786688	1951259329	http://site/test4.html
one	917760	1951259329	http://site/test4.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict4  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
test	66048	-1740442857	http://site/test2.html
this	262400	-1740442857	http://site/test2.html
test	524544	-1740442857	http://site/test2.html
page	590080	-1740442857	http://site/test2.html
here	655616	-1740442857	http://site/test2.html
http	1050880	-1740442857	http://site/test2.html
site	1116160	-1740442857	http://site/test2.html
html	1246720	-1740442857	http://site/test2.html
this	65792	-261021753	http://site/test1.txt
text	262400	-261021753	http://site/test1.txt
file	327936	-261021753	http://site/test1.txt
http	657664	-261021753	http://site/test1.txt
site	722944	-261021753	http://site/test1.txt
http	67840	0	http://site/
site	133120	0	http://site/
test	66048	108051514	http://site/test1.html
this	262400	108051514	http://site/test1.html
test	524544	108051514	http://site/test1.html
page	590080	108051514	http://site/test1.html
here	655616	108051514	http://site/test1.html
4444	1179904	108051514	http://site/test1.html
http	3082496	108051514	http://site/test1.html
site	3147776	108051514	http://site/test1.html
html	3278336	108051514	http://site/test1.html
test	66048	1305968173	http://site/test3.html
this	262400	1305968173	http://site/test3.html
test	524544	1305968173	http://site/test3.html
more	655616	1305968173	http://site/test3.html
http	854272	1305968173	http://site/test3.html
site	919552	1305968173	http://site/test3.html
html	1050112	1305968173	http://site/test3.html
test	66048	1951259329	http://site/test4.html
this	262400	1951259329	http://site/test4.html
test	524544	1951259329	http://site/test4.html
page	590080	1951259329	http://site/test4.html
here	655616	1951259329	http://site/test4.html
http	1247488	1951259329	http://site/test4.html
site	1312768	1951259329	http://site/test4.html
html	1443328	1951259329	http://site/test4.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict5  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
title	197120	-1740442857	http://site/test2.html
third	852224	-1740442857	http://site/test2.html
test2	1181184	-1740442857	http://site/test2.html
title	459008	-261021753	http://site/test1.txt
test1	787968	-261021753	http://site/test1.txt
title	197120	108051514	http://site/test1.html
first	459008	108051514	http://site/test1.html
55555	1245440	108051514	http://site/test1.html
test1	3212800	108051514	http://site/test1.html
title	197120	1305968173	http://site/test3.html
third	459008	1305968173	http://site/test3.html
tests	721152	1305968173	http://site/test3.html
test3	984576	1305968173	http://site/test3.html
title	197120	1951259329	http://site/test4.html
third	852224	1951259329	http://site/test4.html
test4	1377792	1951259329	http://site/test4.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict6  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
second	459008	-1740442857	http://site/test2.html
second	852224	108051514	http://site/test1.html
666666	1310976	108051514	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict7  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
7777777	1376512	108051514	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict8  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
88888888	1442048	108051514	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict9  d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
available	524544	-261021753	http://site/test1.txt
999999999	1507584	108051514	http://site/test1.html
available	786688	1305968173	http://site/test3.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict10 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
aaaaaaaaaa	1573120	108051514	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict11 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
bbbbbbbbbbb	1638656	108051514	http://site/test1.html
SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict12 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
cccccccccccc	1704192	108051514	http://site/test1.html
SQL>SQL>'SELECT d.word,d.intag,u.crc32,u.url FROM dict32 d, url u WHERE u.rec_id=d.url_id ORDER BY u.crc32,d.intag'
ïâýåóô×åîîùå	983296	-1740442857	http://site/test2.html
abcdefghijklmnopqrstuvwxyz	590080	-261021753	http://site/test1.txt
hhhhhhhhhhhhhhhhh	2031872	108051514	http://site/test1.html
iiiiiiiiiiiiiiiiii	2097408	108051514	http://site/test1.html
jjjjjjjjjjjjjjjjjjj	2162944	108051514	http://site/test1.html
kkkkkkkkkkkkkkkkkkkk	2228480	108051514	http://site/test1.html
lllllllllllllllllllll	2294016	108051514	http://site/test1.html
mmmmmmmmmmmmmmmmmmmmmm	2359552	108051514	http://site/test1.html
nnnnnnnnnnnnnnnnnnnnnnn	2425088	108051514	http://site/test1.html
oooooooooooooooooooooooo	2490624	108051514	http://site/test1.html
ppppppppppppppppppppppppp	2556160	108051514	http://site/test1.html
qqqqqqqqqqqqqqqqqqqqqqqqqq	2621696	108051514	http://site/test1.html
rrrrrrrrrrrrrrrrrrrrrrrrrrr	2687232	108051514	http://site/test1.html
ssssssssssssssssssssssssssss	2752768	108051514	http://site/test1.html
ttttttttttttttttttttttttttttt	2818304	108051514	http://site/test1.html
uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu	2883840	108051514	http://site/test1.html
vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv	2949376	108051514	http://site/test1.html
wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww	3014912	108051514	http://site/test1.html
îáùåñòâåííûå	1179904	1951259329	http://site/test4.html
SQL>
