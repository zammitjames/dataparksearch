//////////////////////////////////////////
function utf8_encode ( string ) {
    string = (string+'').replace(/\r\n/g, "\n").replace(/\r/g, "\n");
 
    var utftext = "";
    var start, end;
    var stringl = 0;
 
    start = end = 0;
    stringl = string.length;
    for (var n = 0; n < stringl; n++) {
        var c1 = string.charCodeAt(n);
        var enc = null;
 
        if (c1 < 128) {
            end++;
        } else if((c1 > 127) && (c1 < 2048)) {
            enc = String.fromCharCode((c1 >> 6) | 192) + String.fromCharCode((c1 & 63) | 128);
        } else {
            enc = String.fromCharCode((c1 >> 12) | 224) + String.fromCharCode(((c1 >> 6) & 63) | 128) + String.fromCharCode((c1 & 63) | 128);
        }
        if (enc != null) {
            if (end > start) {
                utftext += string.substring(start, end);
            }
            utftext += enc;
            start = end = n+1;
        }
    }
    if (end > start) {
        utftext += string.substring(start, string.length);
    }
 
    return utftext;
}
function SuggestCopyContent(domDoc, uri) {
 	var destId = 'search_suggest';
 	var dest = document.getElementById ? document.getElementById(destId) : (document.all ? document.all[destId] : null);
	var clie = document.getElementById ? document.getElementById('q') : (document.all ? document.all['q'] : null);
	if (!dest) return;
 	if (dest.innerHTML && domDoc) {
		dest.innerHTML = domDoc + '&nbsp;';
		if (clie) dest.style.width = clie.clientWidth + 'px';
		dest.style.display = 'block';
	} else {
		dest.style.display = 'none';
	}
}
var SuggestLoader = new HTMLHttpRequest('SuggestLoader', SuggestCopyContent);
function searchSuggest() { with(SuggestLoader) {
	if (xmlhttp.readyState == 4 || xmlhttp.readyState == 0) {	
 		var clie = document.getElementById ? document.getElementById('q') : (document.all ? document.all['q'] : null);
		if (!clie) return
		var str = clie.value;
		if (str.length > 0) {
 			str = escape(utf8_encode(str.toString()));
			SuggestLoader.load('/cgi-bin/search.cgi?q=' + str + '&m=all&sp=1&sy=1&&GroupBySite=no&s=IRPD&&ps=10&tmplt=suggest.htm&label=qsimilar');
		}
	}
}}
//Mouse over function
function suggestOver(div_value) {
	div_value.className = 'suggest_link_over';
}
//Mouse out function
function suggestOut(div_value) {
	div_value.className = 'suggest_link';
}
//Click function
function setSearch(value) {
 	var destId = 'search_suggest';
 	var dest = document.getElementById ? document.getElementById(destId) : (document.all ? document.all[destId] : null);
	var clie = document.getElementById ? document.getElementById('q') : (document.all ? document.all['q'] : null);
	clie.value = value.replace(/<[^>]*>/g, "");
	dest.style.display='none';
	clie.focus();
}

