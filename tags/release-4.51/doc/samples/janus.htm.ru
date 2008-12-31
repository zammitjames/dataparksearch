<!--variables
DBAddr		searchd://v.sochi.org.ru/
DBAddr		searchd://v.sochi.org.ru/?label=sochi
StoredocURL     /cgi-bin/storedoc.cgi
LocalCharset 	koi8-r
BrowserCharset  UTF-8
Locale ru_RU.UTF-8
HTTPHeader "Cache-control: public"

#Include i18n/ru.koi8-r.lang
ReplaceVar 43N39E.lng "43&deg;с.ш. 39&deg;в.д."
ReplaceVar all.lng "все слова"
ReplaceVar any.lng "любое из слов"
ReplaceVar AU.lng "Поиск по Австралии"
ReplaceVarLcs BE.lng "����� �� �������"
ReplaceVar Books.lng "Книги на 43&deg;с.ш. 39&deg;в.д."
ReplaceVar Compr.lng "Поиск по серверам, посвященным сжатию данных"
ReplaceVar Justine.lng "Страницы Интернет о Жюстин Энен-Арден"
ReplaceVar Dpsearch.lng "Сайт DataparkSearch"
ReplaceVar near.lng "все слова рядом"
ReplaceVar NZ.lng "Поиск по Новой Зеландии"
ReplaceVar Oregon.lng "Поиск по университету Орегона"
ReplaceVar Search.lng "Поиск"
ReplaceVar SearchFor.lng "Искать"
ReplaceVar bool.lng "логический запрос"
ReplaceVar error.lng "Ошибка!"
ReplaceVar noquery.lng "Вы не задали ни одного слова для поиска."
ReplaceVar notfound.lng "По данному запросу ничего не найдено."
ReplaceVar try.lng "Попробуйте упростить запрос или проверьте правописание."
ReplaceVar langany.lng "любой язык"
ReplaceVar langru.lng "русский язык"
ReplaceVar langen.lng "английский язык"
ReplaceVar langde.lng "немецкий язык"
ReplaceVar langfr.lng "французский язык"
ReplaceVar langzh.lng "китайский язык"
ReplaceVar langko.lng "корейский язык"
ReplaceVar langth.lng "тайский язык"
ReplaceVar langja.lng "японский язык"
ReplaceVar gr.lng "группировать по сайтам"
ReplaceVar nogr.lng "не группировать"
ReplaceVar syn.lng "с синонимами"
ReplaceVar nosyn.lng "без синонимов"
ReplaceVar doc.lng "во всём документе"
ReplaceVar ref.lng "в реферате"
ReplaceVar tit.lng "в заголовке"
ReplaceVar body.lng "в теле"
ReplaceVar mp3.lng "в тэгах MP3"
ReplaceVar alldocs.lng "все документы"
ReplaceVar lastweek.lng "за последнюю неделю"
ReplaceVar last2w.lng "за последние две недели"
ReplaceVar lastmonth.lng "за последний месяц"
ReplaceVar last3m.lng "за последние 3 месяца"
ReplaceVar last6m.lng "за последние 6 месяцев"
ReplaceVar lastyear.lng "за последний год"
ReplaceVar exact.lng "точные слова"
ReplaceVar wforms.lng "варианты слов"
ReplaceVar otherse.lng "в других поисковых системах"
ReplaceVar yandex.lng "Яндекс"
ReplaceVar rambler.lng "Рамблер"
ReplaceVar nigma.lng "Нигма"
ReplaceVar datapark.lng "ДатаПарк"
ReplaceVar usingdp.lng "Испольуя DataparkSearch"
ReplaceVar showndocs.lng "Показаны документы"
ReplaceVar iz.lng "из"
ReplaceVar found.lng "найденных"
ReplaceVar stime.lng "Время поиска"
ReplaceVar sec.lng "сек."
ReplaceVar results.lng "Результаты поиска"
ReplaceVar doyoumean.lng "Имеется в виду"
ReplaceVar pages.lng "Страницы"
ReplaceVar rssres.lng "RSS-поток самых свежих результатов по запросу"
ReplaceVar sortedby.lng "отсортировано по"
ReplaceVar isort.lng "важности"
ReplaceVar rsort.lng "соответствию"
ReplaceVar dsort.lng "дате"
ReplaceVar psort.lng "популярности"
ReplaceVar notitle.lng "[без заголовка]"
ReplaceVar artist.lng "Исполнитель"
ReplaceVar album.lng "Альбом"
ReplaceVar song.lng "Композиция"
ReplaceVar year.lng "Год"
ReplaceVar summary.lng "Реферат"
ReplaceVar stcopy.lng "Сохранённая копия"
ReplaceVar allresfrom.lng "Все результаты с"
ReplaceVar onmap.lng "на карте"
ReplaceVar price.lng "Цена"
ReplaceVarLcs phone.lng "���."
ReplaceVar rur.lng "руб."
ReplaceVar prev.lng "Пред."
ReplaceVar next.lng "След."
ReplaceVar instplugin.lng "Установить поисковый плагин"
ReplaceVar mkreport.lng "Написать отзыв"
ReplaceVar bytes.lng "байт"
ReplaceVar score.lng "Рейтинг"
ReplaceVar ToFind.lng "Найти"
ReplaceVarLcs extended.lng "����������� �����"
ReplaceVarLcs Sochi.org.ru.lng "�������� ����"
ReplaceVarLcs wholinks.lng "��� ���������"
ReplaceVarLcs qwords.lng "����� �������"

MinWordLength 1
MaxWordLength 64
DetectClones no
ExcerptSize 156
ExcerptPadding 36
HlBeg <span class="rhl">
HlEnd </span>
GrBeg <div class="group">
GrEnd </div>
DateFormat %d.%m.%Y
m near
sp 1
sy 0
-->

<!--top-->
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<HTML lang="ru"><!IF NAME="c" CONTENT="01"><!IF NAME="label" CONTENT=""><!SET NAME="label" CONTENT="sochi"><!ENDIF><!ENDIF>
  <HEAD>
    <TITLE>$(43N39E.lng): $&(q)</TITLE>
    <link href="http://sochi.org.ru/favicon.ico" rel="shortcut icon" type="image/x-icon">
    <link rel="alternate" type="application/rss+xml" title="RSS 2.0" href="$(self)?q=$%(q)&amp;c=$&(c)&amp;site=$&(site)&amp;m=$&(m)&amp;sp=$&(sp)&amp;sy=$&(sy)&amp;&amp;s=$&(s)&amp;label=$&(label)&amp;tmplt=rss8.htm.ru">
    <script type="text/javascript"><!--
      var dpstate = 'h';
      var ext1Content = null;
      var ext2Content = null;
      var exthContent = null;
      var mainContent = null;
      var formContent = null;
      function dp_init() {
	mainContent = document.getElementById('ext0');
	formContent = document.getElementById('fo0');
	ext1Content = document.getElementById('ext1');
	ext2Content = document.getElementById('ext2');
	exthContent = document.getElementById('exth');
	try {
		mainContent.removeChild(ext1Content);
		mainContent.removeChild(ext2Content);
	} catch (ex) {
	}
	ext1Content.style.visibility = 'visible';
	ext2Content.style.visibility = 'visible';
      }
      function swap() {
	if (dpstate == 'h') {
		mainContent.appendChild(ext1Content);
		mainContent.appendChild(ext2Content);
		formContent.removeChild(exthContent);
		dpstate = 'v';
	} else {
	        mainContent.removeChild(ext1Content);
        	mainContent.removeChild(ext2Content);
		formContent.appendChild(exthContent);
		dpstate = 'h';
	}
	return false;
      }
      function clk(id,pos){
	var u = new Date().getTime();
        var i = new Image();
	i.src="http://s.sochi.org.ru/cgi-bin/c.pl?id="+id+"&pos="+pos+'&u='+u;return true;
      }
      //--></script>
	<style type="text/css">
		BODY { margin-left: 1em; margin-right: 1em; }
		SELECT { width: 15em; }
	</style>
    <script type="text/javascript">
function myCopyContent(domDoc, uri) {
 var destId = 'resultArea';
 var dest = document.getElementById ? document.getElementById(destId) :
  (document.all ? document.all[destId] : null);
 if (!dest) return;
 if (dest.innerHTML) dest.innerHTML = domDoc;
}
function myCopyContent2(domDoc, uri) {
 var destId = 'similarArea';
 var dest = document.getElementById ? document.getElementById(destId) :
  (document.all ? document.all[destId] : null);
 if (!dest) return;
 if (dest.innerHTML) dest.innerHTML = domDoc;
}
<!IF NAME="c" CONTENT="01">
function myCopyContent3(domDoc, uri) {
 var destId = 'rutubeArea';
 var dest = document.getElementById ? document.getElementById(destId) :
  (document.all ? document.all[destId] : null);
 if (!dest) return;
 if (dest.innerHTML) dest.innerHTML = domDoc;
}
<!ENDIF>
    </script>
    <link href="/css/therm2.css" rel="stylesheet" type="text/css">
 </HEAD>
<!-- BrowserCharset: $(BrowserCharset) -->
<body>

<div id="form">
<center>
<table width="100%" border="0" cellpadding="0" cellspacing="0" align="center" style="background: url(/tdbg2.png) repeat-x #f4f4f4;">

<tr>
<td class="headruler" align="left" valign="top">&nbsp;<b>
<!IF NAME="c" CONTENT="0B09"><a href="http://books.43n39e.ru/">$(Books.lng)</a>
<!ELIF NAME="c" CONTENT="01"><a href="http://sochi.org.ru/">$(Sochi.org.ru.lng)</a>
<!ELIF NAME="c" CONTENT="09"><a href="http://www.43n39e.ru/gday">$(AU.lng)</a>
<!ELIF NAME="c" CONTENT="0F"><a href="http://www.43n39e.ru/nz">$(NZ.lng)</a>
<!ELIF NAME="c" CONTENT="0K"><a href="http://www.43n39e.ru/be">$(BE.lng)</a>
<!ELIF NAME="c" CONTENT="0D"><a href="http://www.43n39e.ru/oregon">$(Oregon.lng)</a>
<!ELIF NAME="c" CONTENT="04"><a href="http://www.maxime.net.ru/">$(Compr.lng)</a>
<!ELIF NAME="c" CONTENT="06"><a href="http://justine.43n39e.ru/">$(Justine.lng)</a>
<!ELIF NAME="site" CONTENT="-572280698"><a href="http://www.dataparksearch.org/">$(Dpsearch.lng)</a> 
<!ELSE><a href="http://www.43n39e.ru/">$(43N39E.lng)</a>
<!ENDIF>
</b></td>
</tr>

<tr>
<td valign="middle" align="center">

	    <form id="fo0" method="GET" action="$(self)" enctype="application/x-www-form-urlencoded">
	      <input type="hidden" name="ps" value="10">
	      <input type="hidden" name="np" value="0">
	      <input type="hidden" name="dt" value="back">
	      <input type="hidden" name="t" value="$&(t)">
	      <input type="hidden" name="c" value="$&(c)">
	      <input type="hidden" name="tmplt" value="janus.htm.ru">
	      <input type="hidden" name="label" value="$&(label)">
	      <input type="hidden" name="s" value="$&(s)">
<div id="exth">
	<input type="hidden" name="m" value="$&(m)">
	<input type="hidden" name="g" value="$&(g)">
	<input type="hidden" name="GroupBySite" value="$&(GroupBySite)">
	<input type="hidden" name="sy" value="$&(sy)">
	<input type="hidden" name="rm" value="$&(rm)">
	<input type="hidden" name="wf" value="$&(wf)">
	<input type="hidden" name="dp" value="$&(dp)">
	<input type="hidden" name="sp" value="$&(sp)">
</div>
<!IF NAME="site" CONTENT="-572280698">
	<input type="hidden" name="site" value="-572280698">
<!ENDIF>

<table width="100%" border="0" cellpadding="2" cellspacing="0">
<table width="100%" border="0" cellpadding="2" cellspacing="0">
<tbody id="ext0">
<tr>
<td width="100%">
			<table width="100%" border="0" cellpadding="2" cellspacing="0">
			    <tr>
			      <td width="99%" style="vertical-align: middle;">
				  <input class="inputsearch" type="text" name="q" size="60" value="$&(q)" style="width:100%;">
			      </td><td>
				  <input class="inputsearch" type="submit" value="$(ToFind.lng)">
			      </td>
			    </tr>
			    <!--tr>
			      <td class="inputrev">&nbsp;</td>
			    </tr-->
			</table>
</td>
</tr>
<tr><td width="100%" align="right"><small><a href="#" onMouseDown="return swap();">$(extended.lng)</a></small></td></tr>
<tr id="ext1" style="visibility: hidden;">
<td width="100%" align="right">
 <select name="m" class="inputrev">
 <option value="all" selected="$&(m)"> $(all.lng) 
 <option value="near" selected="$&(m)"> $(near.lng) 
 <option value="any" selected="$&(m)"> $(any.lng)
 <option value="bool" selected="$&(m)"> $(bool.lng)
 </select>
<select class="inputrev" name="g">
<OPTION VALUE="" SELECTED="$&(g)">$(langany.lng)</option>
<OPTION VALUE="ru" SELECTED="$&(g)">$(langru.lng)</option>
<OPTION VALUE="en" SELECTED="$&(g)">$(langen.lng)</option>
<OPTION VALUE="de" SELECTED="$&(g)">$(langde.lng)</option>
<OPTION VALUE="fr" SELECTED="$&(g)">$(langfr.lng)</option>
<OPTION VALUE="zh" SELECTED="$&(g)">$(langzh.lng)</option>
<OPTION VALUE="ko" SELECTED="$&(g)">$(langko.lng)</option>
<OPTION VALUE="th" SELECTED="$&(g)">$(langth.lng)</option>
<OPTION VALUE="ja" SELECTED="$&(g)">$(langja.lng)</option>
</select>
<select class="inputrev" name="GroupBySite">
<option value="yes" selected="$&(GroupBySite)">$(gr.lng)</option>
<option value="no"  selected="$&(GroupBySite)">$(nogr.lng)</option>
</select>
<select class="inputrev" name="sy">
<option value="0" selected="$&(sy)">$(nosyn.lng)</option>
<option value="1" selected="$&(sy)">$(syn.lng)</option>
</select><br>
</td></tr>

<tr id="ext2" style="visibility: hidden;">
<td width="100%" align="right">
<select class="inputrev" name="rm">
<option value="2" selected="$&(rm)">full</option>
<option value="1" selected="$&(rm)">fast</option>
<option value="3" selected="$&(rm)">ultra</option>
</select>
<select class="inputrev" name="wf">
<option value="F1F1" selected="$&(wf)">$(doc.lng)</option>
<option value="00000F00" selected="$&(wf)">$(ref.lng)</option>
<option value="000000F0" selected="$&(wf)">$(tit.lng)</option>
<option value="0000000F" selected="$&(wf)">$(body.lng)</option>
<!--option value="33F37000" selected="$&(wf)">$(mp3.lng)</option-->
</select>
<SELECT class="inputrev" NAME="dp">
<OPTION VALUE="0" SELECTED="$&(dp)">$(alldocs.lng)</option>
<OPTION VALUE="7d" SELECTED="$&(dp)">$(lastweek.lng)</option>
<OPTION VALUE="14d" SELECTED="$&(dp)">$(last2w.lng)</option>
<OPTION VALUE="1m" SELECTED="$&(dp)">$(lastmonth.lng)</option>
<OPTION VALUE="3m" SELECTED="$&(dp)">$(last3m.lng)</option>
<OPTION VALUE="6m" SELECTED="$&(dp)">$(last6m.lng)</option>
<OPTION VALUE="1y" SELECTED="$&(dp)">$(lastyear.lng)</option>
</select>
<select class="inputrev" name="sp">
<option value="0" selected="$&(sp)">$(exact.lng)</option>
<option value="1" selected="$&(sp)">$(wforms.lng)</option>
</select>
</td></tr>
</tbody>
</table>
</FORM>
</td>
</tr>
</table>
</center>
</div>
<script type="text/javascript"><!--
dp_init();
//--></script>
<!--/top-->

<!--bottom-->
<div id="bottom">
<br>
<!IFNOT NAME="q" CONTENT="">
<small>$(SearchFor.lng) &laquo;$&(q)&raquo; $(otherse.lng):
<a href="http://www.google.com/search?ie=UTF-8&amp;hl=ru&amp;q=$%(q:utf-8)">Google</a> &#150;
<a href="http://www.yandex.ru/yandsearch?text=$%(q:cp1251)&amp;stype=www">$(yandex.lng)</a> &#150;
<a href="http://www.rambler.ru/srch?oe=1251&set=www&words=$%(q:cp1251)">$(rambler.lng)</a> &#150;
<a href="http://gogo.ru/go?q=$%(q:cp1251)">GoGo</a> &#150;
<a href="http://www.nigma.ru/index.php?0=1&1=1&2=1&3=1&4=1&5=1&6=1&7=1&q=$%(q:cp1251)">$(nigma.lng)</a> &#150;
<a href="http://search.yahoo.com/bin/query?p=$%(q:UTF-8)&amp;ei=UTF-8">Yahoo!</a> &#150;
<a href="http://search.live.com/results.aspx?q=$%(q:UTF-8)">Live.com</a>
</small>
<!ENDIF>
<script type="text/javascript">
function showBooksAndVideo() {
var CommerceLoader = new HTMLHttpRequest('CommerceLoader', myCopyContent);
var SimilarLoader = new HTMLHttpRequest('SimilarLoader', myCopyContent2);
<!IF NAME="c" CONTENT="01">
SimilarLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&s_c=$&(c)&s_sp=$&(sp)&s_sy=$&(sy)&m=any&sp=1&sy=1&p=$&(p)&GroupBySite=no&s=$&(s)&s_GroupBySite=$&(GroupBySite)&ps=5&tmplt=qsimilar8.htm.ru&label=qsimilar&s_label=$&(label)');
var RutubeLoader = new HTMLHttpRequest('RutubeLoader', myCopyContent3);
RutubeLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&m=$&(m)&g=$&(g)&sp=$&(sp)&sy=$&(sy)&p=$&(p)&GroupBySite=no&s=$&(s)&&ps=3&tmplt=rutube8.htm.ru&label=rutube');
CommerceLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&m=$&(m)&g=$&(g)&sp=1&sy=1&p=$&(p)&GroupBySite=no&s=$&(auxs)&link=$&(link)&ps=5&tmplt=phones8.htm.ru&label=phones');
<!ELSE>
SimilarLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&s_c=$&(c)&s_sp=$&(sp)&s_sy=$&(sy)&m=any&sp=1&sy=1&p=$&(p)&GroupBySite=no&s=$&(s)&s_GroupBySite=$&(GroupBySite)&ps=7&tmplt=qsimilar8.htm.ru&label=qsimilar');
//CommerceLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&m=$&(m)&c=$&(c)&g=$&(g)&sp=$&(sp)&sy=$&(sy)&p=$&(p)&GroupBySite=$&(GroupBySite)&s=$&(auxs)&link=$&(link)&ps=7&tmplt=duo8.htm.ru');
CommerceLoader.load('/cgi-bin/search.cgi?q=$%(q:utf-8)&m=$&(m)&g=$&(g)&sp=$&(sp)&sy=$&(sy)&p=$&(p)&GroupBySite=no&s=$&(s)&ps=5&tmplt=rutube8.htm.ru&label=rutube');
<!ENDIF>
}
</script>
<table border="0" width="100%">
<tr>
<td align="center" valign="top">
	&nbsp;
</td>
</tr>
<tr><td align="center" valign="middle">
<small>(C)2008, &laquo;<a href="http://www.datapark.ru/">$(datapark.lng)</a>&raquo;<br>
<a href="http://www.dataparksearch.org/">$(usingdp.lng)</a></small><br>
</td></tr>
</table>
</div>
<script type="text/javascript" src="/js/htmlhttprequest_commented.js"></script>
<script type="text/javascript">showBooksAndVideo();</script>
</body>
</html>
<!--/bottom-->

<!--restop-->
<div class="news">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td colspan="2" align="left" class="serp">
<small>&nbsp;$(showndocs.lng) <b>$(first)</b>-<b>$(last)</b> $(iz.lng) <b>$(total)</b> $(found.lng).</small>
<td>
<td width="50%" align="right">
<small>$(stime.lng):&nbsp;<b>$(SearchTime)</b> $(sec.lng)&nbsp;</small>
</td></tr>
</table>
</div>
<div class="news">
<small>&nbsp;<b>$(results.lng):</b>&nbsp;$(WS)</small>
<!IFNOT NAME="Suggest_q" VALUE=""><br><small>&nbsp;<b>$(doyoumean.lng) &laquo;<a class="newcat" href="$(Suggest_url)">$(Suggest_q)</a>&raquo; ?</b></small>
<!ENDIF>
</div>
<div class="bottom">
<table width="100%" border="0" cellspacing="3" cellpadding="0"><tr><td valign="bottom">
<table align="center" border="0" cellspacing="0" cellpadding="3"><tr><td>$(pages.lng):</td>$(NL)$(NB)$(NR)</tr></table></td>
<td width="180" align="right" rowspan="2">&nbsp;
</td></tr><tr><td align="left" valign="middle">$(sortedby.lng):&nbsp;
<!IF NAME="s" CONTENT="DRP"><!SET NAME="auxs" CONTENT="PRD">
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(dsort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a><!ELIF NAME="s" CONTENT="IRPD"><!SET NAME="auxs" CONTENT="RPD">
<span class="hl">$(isort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a><!ELIF NAME="s" CONTENT="PRD"><!SET NAME="auxs" CONTENT="AD">
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(psort.lng)</span><!ELSE><!SET NAME="auxs" CONTENT="DA">
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(rsort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a>
<!ENDIF></td></tr></table>
</div>
<div id="dir">
<table width="100%" cellpadding="5" cellspacing="0" border="0">
<tr>
<td rowspan="2" valign="top" align="left" style="padding-right: 20px;">
<!-- a -->
<!--/restop-->


<!--res-->
<!-- m -->
<div style="width: 40em;">
<div class="serp1"><i class="serp1" style="background-image:url(http://favicon.yandex.ru/favicon/$(url.host))"></i>
<span style="margin-left:20px;">&#8203;</span>
<!-- $(DP_ID), site_id: $(Site_ID), ST: $(ST) --><!IF NAME="Content-Type" CONTENT="application/msword"><b class="mimetype">[DOC]</b>&nbsp;
<!ELIKE NAME="Content-Type" CONTENT="audio/*"><b class="mimetype">[MP3]</b>&nbsp;
<!ELIF NAME="Content-Type" CONTENT="application/pdf"><b class="mimetype">[PDF]</b>&nbsp;
<!ELIF NAME="Content-Type" CONTENT="application/vnd.ms-excel"><b class="mimetype">[XLS]</b>&nbsp;
<!ELIF NAME="Content-Type" CONTENT="application/rtf"><b class="mimetype">[RTF]</b>&nbsp;
<!ELIF NAME="Content-Type" CONTENT="text/rtf"><b class="mimetype">[RTF]</b>&nbsp;
<!ELIKE NAME="Content-Type" CONTENT="image/*"><b class="mimetype">[IMG]</b>&nbsp;
<!ELIF NAME="Content-Type" CONTENT="application/x-shockwave-flash"><b class="mimetype">[SWF]</b>&nbsp;
<!ENDIF><!IF NAME="url.host" CONTENT="www.books.ru"><a onmousedown="return clk('$(DP_ID)','$(Order)');" class="restop" href="$(URL)?partner=datapark">
<!ELIF NAME="url.host" CONTENT="www.bolero.ru"><a onmousedown="return clk('$(DP_ID)','$(Order)');" class="restop" href="http://www.bolero.ru//cgi-bin/superptr.cgi?partner=datapark&amp;new=1&amp;uri=$(URL.path)$(URL.file)">
<!ELIF NAME="url.host" CONTENT="www.ozon.ru"><a onmousedown="return clk('$(DP_ID)','$(Order)');" class="restop" href="$(URL)?partner=datapark">
<!ELSE><a id="link$(Order)" onmousedown="return clk('$(DP_ID)','$(Order)');" class="restop" href="$(URL)"><!ENDIF><!--
 --><!IF NAME="Title" CONTENT="[no title]">
	 <!IF NAME="MP3.Artist" CONTENT="">$(notitle.lng)<!ELSE>$(artist.lng): $&(MP3.Artist)<!ENDIF>
	 <!ELIKE NAME="Title" CONTENT="/tmp/ind*">$(notitle.lng)
    <!ELSE>$&(Title:72)<!ENDIF><!--
--></a></div>
<div class="serp2" style="margin-left:20px;">
<!IF NAME="url.host" CONTENT="www.books.ru">
<table border="0" cellpadding="0" cellspacing="0"><tr><td align="center" valign="middle">
<a onmousedown="return clk('$(DP_ID)','$(Order)');" href="$(URL)?partner=datapark"><img src="http://www.books.ru/img/$(url.file)s.jpg" border="0"></a>
</td><td align="left" valign="top" style="padding-left: 0.5em;"><!ENDIF>
<!IF NAME="MP3.Album" CONTENT=""> <!ELSE><span class="result">$(album.lng): </span> 
$&(MP3.Album)<br><!ENDIF>
<!IF NAME="MP3.Song" CONTENT=""> <!ELSE><span class="result">$(song.lng): </span>
$&(MP3.Song)<br><!ENDIF>
<!IF NAME="MP3.Year" CONTENT=""> <!ELSE><span class="result">$(year.lng): </span>
$&(MP3.Year)<br><!ENDIF>
<!IF NAME="Body" CONTENT=""><!IFNOT NAME="sea" CONTENT=""><div class="serp3"><small class="restop">$(summary.lng):</small> <small>$&(sea:600)</small></div><!ENDIF><!ELSE><small>$&(Body:350)</small><br><!ENDIF></div>
<div class="serp2"><small class="result">$&(URL:40)<br>$(Last-Modified) - $(FancySize) $(bytes.lng) - $(Content-Language)<!-- - $(Charset)--></small>
<small class="result">- $(score.lng): $(Score) [$(Pop_Rank)]
<!IFNOT NAME="Price" CONTENT=""><span class="result"> - $(price.lng): </span>$(Price) $(rur.lng)<!ENDIF>
<!IFNOT NAME="Phone" CONTENT=""><span class="result"> - $(phone.lng): </span>$(Phone)<!ENDIF>
<br>
	      <!IFNOT NAME="stored_href" CONTENT=""><a onmousedown="return clk('$(DP_ID)','$(Order)');" href="$(stored_href)">$(stcopy.lng)</a><!ENDIF>
	      <!IFNOT NAME="sitelimit_href" CONTENT=""> &nbsp;&nbsp;<a href="$(self)$(sitelimit_href)">$(allresfrom.lng) $(url.host)
				<!IFNOT NAME="PerSite" CONTENT="0">($(PerSite))<!ENDIF><!--
--></a>
	      <!ENDIF>
<!IFNOT NAME="geo.lat" CONTENT=""> &nbsp;&nbsp;<a href="/cgi-bin/gmap.cgi.ru?q=$%(q)&amp;c=$&(c)&amp;m=$&(m)&amp;sp=$&(sp)&amp;sy=$&(sy)&amp;s=$&(s)&amp;tmplt=gmap43.htm.ru&amp;s_tmplt=$&(tmplt)&amp;title=$%(title)&amp;geo.lat=$&(geo.lat)&amp;geo.lon=$&(geo.lon)">$(onmap.lng)</a><!ENDIF>
&nbsp;<a href="$(self)?q=$%(q)&amp;c=$&(c)&amp;m=$&(m)&amp;sp=$&(sp)&amp;sy=$&(sy)&amp;s=$&(s)&amp;label=$&(label)&amp;tmplt=janus.htm.ru&amp;link=$(DP_ID)">$(wholinks.lng)</a>
</small>
<!IF NAME="url.host" CONTENT="www.books.ru">
</td></tr></table><!ENDIF>
</div>
$(CL)</div>
<!-- n -->
<!--/res-->

<!--clone-->
<div><li><A HREF="$(URL)">$(URL:50)</A>,&nbsp; $(Last-Modified)</div>
<!--/clone-->

<!--navleft-->
<TD><A class="leftmenu" HREF="$(self)$(NH)">$(prev.lng)</A></TD>
<!--/navleft-->

<!--navbar0-->
<TD><b class="hl">$(NP)</b></TD>
<!--/navbar0-->

<!--navbar1-->
<TD><A class="leftmenu" HREF="$(self)$(NH)">$(NP)</A></TD>
<!--/navbar1-->

<!--navright-->
<TD><A class="leftmenu" HREF="$(self)$(NH)">$(next.lng)</A></TD>
<!--/navright-->

<!--navigator-->
<TABLE BORDER="0"><TR><TD>$(pages.lng):</TD>$(NL) $(NB) $(NR)</TR></TABLE>
<!--/navigator-->

<!--resbot-->
<!-- z -->
</td>
<td rowspan="2" valign="top" align="left" style="width:14em; border-right:1px solid #f4f4f4;">
<br><div id="similarArea">&nbsp;</div>
<div id="resultArea">&nbsp;</div>
<!IF NAME="c" CONTENT="01"><div id="rutubeArea">&nbsp;</div><!ENDIF>
</td>
<td width="160" valign="top" align="right" style="border-left: 1px solid #f4f4f4">
<br><br>
<!-- ads here -->
<!IFLIKE NAME="ENV.SERVER_NAME" CONTENT="*sochi.org.ru"> <script type="text/javascript">
   var begun_auto_pad = 90069626; // pad id
   var begun_block_id = 112889598;
</script>
<script src="http://autocontext.begun.ru/autocontext2.js" type="text/javascript">
</script>
<!ELSE>  <script type="text/javascript">
   var begun_auto_pad = 90026481; // pad id
   var begun_block_id = 112888603;
</script>
<script src="http://autocontext.begun.ru/autocontext2.js" type="text/javascript">
</script>
<!ENDIF>
</td>
</tr>
<tr><td width="160" align="center" valign="bottom">
<script type="text/javascript" src="/install.js"></script>
<a class="bottom" href="javascript:add_dpSearch()"><small>$(instplugin.lng)</small></a>
<br>
<a class="bottom" href="http://sochi.org.ru/makereport.html">$(mkreport.lng)</a>
<br><br>
<!--LiveInternet counter--><script type="text/javascript"><!--
document.write('<a href="http://www.liveinternet.ru/click" '+
'target=_blank><img src="http://counter.yadro.ru/hit?t26.5;r'+
escape(document.referrer)+((typeof(screen)=='undefined')?'':
';s'+screen.width+'*'+screen.height+'*'+(screen.colorDepth?
screen.colorDepth:screen.pixelDepth))+';u'+escape(document.URL)+
';'+Math.random()+
'" alt="" title="LiveInternet: " '+
'border=0 width=88 height=15><\/a>')//--></script><!--/LiveInternet-->
</td></tr></table>
</div>
<div class="bottom">
<br>
$(sortedby.lng):&nbsp;
<!IF NAME="s" CONTENT="DRP">
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(dsort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a><!ELIF NAME="s" CONTENT="IRPD">
<span class="hl">$(isort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a><!ELIF NAME="s" CONTENT="PRD">
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=RPD">$(rsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(psort.lng)</span><!ELSE>
<a href="$(FirstPage)&s=IRPD">$(isort.lng)</a>&nbsp;|&nbsp;<span class="hl">$(rsort.lng)</span>&nbsp;|&nbsp;<a href="$(FirstPage)&s=DRP">$(dsort.lng)</a>&nbsp;|&nbsp;<a href="$(FirstPage)&s=PRD">$(psort.lng)</a>
<!ENDIF><br><br>
<CENTER><TABLE BORDER="0" cellspacing="0" cellpadding="3"><TR><td>$(pages.lng):</td>$(NL) $(NB) $(NR)</TR></TABLE></CENTER>
</div>
<div class="form">
<br>
<center>
<table width="100%" border="0" cellpadding="2" cellspacing="0" align="center" style="background-image: url(/tdbg2.png);">
<tr>
<td valign="middle" align="center">

	<form method="GET" action="$(self)" enctype="application/x-www-form-urlencoded">
	<input type="hidden" name="ps" value="10">
	<input type="hidden" name="np" value="0">
	<input type="hidden" name="dt" value="back">
	<input type="hidden" name="t" value="$&(t)">
	<input type="hidden" name="c" value="$&(c)">
	<input type="hidden" name="s" value="$&(s)">
	<input type="hidden" name="tmplt" value="janus.htm.ru">
	<input type="hidden" name="label" value="$&(label)">
	<input type="hidden" name="g" value="$&(g)">
	<input type="hidden" name="GroupBySite" value="$&(GroupBySite)">
	<input type="hidden" name="sy" value="$&(sy)">
	<input type="hidden" name="sp" value="$&(sp)">
	<input type="hidden" name="wf" value="$&(wf)">
	<input type="hidden" name="dp" value="$&(dp)">

<table border="0" cellpadding="2" cellspacing="0">
<tr>
<td width="100%">
 &nbsp;<b class="inputrev">$(searchfor.lng):</b> <input class="inputrev" type="text" name="q" size="50" value="$&(q)">
 <INPUT class="inputrev" TYPE="submit" VALUE="&gt;&gt;&gt;">
 &nbsp;
 <select name="m" class="inputrev">
 <option value="all" selected="$&(m)">$(all.lng)</option> 
 <option value="near" selected="$&(m)">$(near.lng)</option> 
 <option value="any" selected="$&(m)">$(any.lng)</option>
 <option value="bool" selected="$&(m)">$(bool.lng)</option>
 </select>
 </td>
</tr>
</table>
</FORM>
</td>
</tr>
</table>
</center>
</div>
<!--/resbot-->


<!--notfound-->
<div class="news">
<table width="100%" border="0" cellspacing="0" cellpadding="0">
<tr><td align="right">
<small>$(stime.lng):&nbsp;$(SearchTime) $(sec.lng)&nbsp;</small>
</td></tr>
</table>
</div>
<div class="news">
<small>&nbsp;$(results.lng):&nbsp;$(W)</small>
<!IFNOT NAME="Suggest_q" VALUE=""><br><small>&nbsp;<b>$(doyoumean.lng) &laquo;<a class="newcat" href="$(Suggest_url)">$(Suggest_q)</a>&raquo; ?</b></small>
<!ENDIF>
<br>
<br>
<b>$(notfound.lng)</b>
$(try.lng)
</div>
<br><br>
<table border="0" cellspacing="0" cellpadding="10"><tr>
<td><div id="similarArea" style="width: 14em;">&nbsp;</div></td>
<td><div id="resultArea" style="width="14em;">&nbsp;</div></td>
<!IF NAME="c" CONTENT="01"><td><div id="rutubeArea" style="width: 14em; float:right;">&nbsp;</div></td><!ENDIF>
</tr></table>
<br>
<!--/notfound-->

<!--noquery-->
<center>
$(noquery.lng)
</center>
<!--/noquery-->

<!--error-->
<center>
<font color="#FF0000">$(error.lng)</font>
<p>
<b>$(E)</b>
</center>
<!--/error-->
