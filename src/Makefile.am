SUBDIRS = .


if PORTS
dps_sharedir=$(pkgdatadir)
else
dps_sharedir=$(datadir)
endif
#AM_CFLAGS = -DDPS_CONF_DIR=\"@sysconfdir@\" -DDPS_VAR_DIR=\"@localstatedir@\" -DDPS_SHARE_DIR=\"$(dps_sharedir)\"
CFLAGS = @CFLAGS@ -DDPS_CONF_DIR=\"@sysconfdir@\" -DDPS_VAR_DIR=\"@localstatedir@\" -DDPS_SHARE_DIR=\"$(dps_sharedir)\"


if THREADSLESS
extra = libdpsearch_mod.la
endif


lib_LTLIBRARIES = libdpsearch.la libdpcharset.la $(extra)

noinst_HEADERS = uconv-8bit.h
bin_PROGRAMS = search.cgi storedoc.cgi filler.cgi
sbin_PROGRAMS = @db_PROGS@
noinst_PROGRAMS = @APACHE_MODULE@ @PHP_MODULE@
EXTRA_PROGRAMS= indexer searchd splitter stored dpguesser cached dpconv mod_dpsearch.so dpsearch.so
EXTRA_DIST = mod_dpsearch.c sgml.entities sgml.inc uconv-8bit.inc unidata.ch php.c commands.lst commands.inc timezones.lst timezones.inc

splitter_SOURCES = splitter.c
splitter_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
splitter_LDFLAGS = @PTHREAD_LFLAGS@
splitter_CFLAGS = @PTHREAD_CFLAGS@

searchd_SOURCES = searchd.c
searchd_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
searchd_LDFLAGS = @PTHREAD_LFLAGS@
searchd_CFLAGS = @PTHREAD_CFLAGS@

stored_SOURCES = stored.c
stored_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
stored_LDFLAGS = @PTHREAD_LFLAGS@
stored_CFLAGS = @PTHREAD_CFLAGS@

indexer_SOURCES = main.c
indexer_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
indexer_LDFLAGS = @PTHREAD_LFLAGS@
indexer_CFLAGS = @PTHREAD_CFLAGS@

search_cgi_SOURCES = search.c
search_cgi_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
search_cgi_LDFLAGS = @PTHREAD_LFLAGS@
search_cgi_CFLAGS = @PTHREAD_CFLAGS@

storedoc_cgi_SOURCES = storedoc.c
storedoc_cgi_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
storedoc_cgi_LDFLAGS = @PTHREAD_LFLAGS@
storedoc_cgi_CFLAGS = @PTHREAD_CFLAGS@

filler_cgi_SOURCES = filler.c
filler_cgi_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
filler_cgi_LDFLAGS = @PTHREAD_LFLAGS@
filler_cgi_CFLAGS = @PTHREAD_CFLAGS@

dpguesser_SOURCES = dpguesser.c
dpguesser_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
dpguesser_LDFLAGS = @PTHREAD_LFLAGS@  
dpguesser_CFLAGS = @PTHREAD_CFLAGS@

dpconv_SOURCES = dpconv.c
dpconv_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
dpconv_LDFLAGS = @PTHREAD_LFLAGS@  
dpconv_CFLAGS = @PTHREAD_CFLAGS@

cached_SOURCES = cached.c
cached_LDADD = libdpsearch.la libdpcharset.la @PTHREAD_LDADD@
cached_LDFLAGS = @PTHREAD_LFLAGS@  
cached_CFLAGS = @PTHREAD_CFLAGS@

libdpsearch_la_SOURCES = indexer.c url.c parser.c sql.c spell.c \
log.c proto.c robots.c hrefs.c mutex.c conf.c utils.c cookies.c \
parsedate.c ftp.c socket.c host.c http.c id3.c signals.c \
cache.c boolean.c searchtool.c searchcache.c server.c alias.c env.c \
agent.c word.c parsehtml.c stopwords.c execget.c crossword.c \
doc.c contentencoding.c vars.c db.c sdp.c textlist.c dps_image.c \
result.c synonym.c match.c store.c template.c guesser.c base.c carry.c \
mirror.c sqldbms.c mkind.c chinese.c parsexml.c wild.c hash.c indexertool.c acronym.c sea.c \
efence.c ef_page.c ef_print.c filence.c
#EXTRA_libdpsearch_la_SOURCES = efence.c ef_page.c ef_print.c
libdpsearch_la_LIBADD = @LIBLIB@ @PTHREAD_LDADD@ 
libdpsearch_la_LDFLAGS = -release @DPS_BASE_VERSION@
libdpsearch_la_DEPENDENCIES = libdpcharset.la 

libdpcharset_la_SOURCES = uconv.c unidata.c sgml.c unicode.c xmalloc.c \
uconv-8bit.c uconv-alias.c uconv-big5.c uconv-big5hkscs.c uconv-eucjp.c uconv-euckr.c \
uconv-gb2312.c uconv-gbk.c uconv-sjis.c uconv-sys.c uconv-utf8.c uconv-gujarati.c \
uconv-tscii.c charset-utils.c
libdpcharset_la_LDFLAGS = -release @DPS_BASE_VERSION@ 


sgml.c: sgml.inc

sgml.inc: sgml.entities
	cat sgml.entities | sort -k 2 > sgml.inc

conf.c: commands.inc

commands.inc: commands.lst
	cat commands.lst | sort -fk 1 > commands.inc

utils.c: timezones.inc

timezones.inc: timezones.lst
	cat timezones.lst | sort -fk 1 > timezones.inc


if THREADSLESS
libdpsearch_mod_la_SOURCES = indexer.c url.c parser.c sql.c spell.c \
log.c proto.c robots.c hrefs.c mutex.c conf.c utils.c cookies.c \
parsedate.c ftp.c socket.c host.c http.c id3.c signals.c \
cache.c boolean.c searchtool.c searchcache.c server.c alias.c env.c \
agent.c word.c parsehtml.c stopwords.c execget.c crossword.c \
doc.c contentencoding.c vars.c db.c sdp.c textlist.c dps_image.c \
result.c synonym.c match.c store.c template.c guesser.c base.c \
mirror.c sqldbms.c mkind.c chinese.c parsexml.c wild.c hash.c indexertool.c acronym.c sea.c \
efence.c ef_page.c ef_print.c filence.c
libdpsearch_mod_la_LIBADD = @MODLIB@

if APACHEMODULE
libdpsearch_mod_la_CFLAGS = -DAPACHE@APACHE_VER@ -I@APACHE_INCDIR@ $(APACHE_CFLAGS) $(APACHE_NOTEST_CPPFLAGS) $(APACHE_EXTRA_CPPFLAGS) $(APACHE_EXTRA_CFLAGS)
endif

libdpsearch_mod_la_LDFLAGS = -release @DPS_BASE_VERSION@
libdpsearch_mod_la_DEPENDENCIES = libdpcharset.la

endif


if APACHEMODULE

mod_dpsearch_so_SOURCES=mod_dpsearch.c
mod_dpsearch_so_CFLAGS=$(APACHE_CFLAGS)
if APACHE2
mod_dpsearch_so_LDFLAGS=--shared
endif


if THREADSLESS

mod_dpsearch_so_LDADD = libdpsearch_mod.la libdpcharset.la @APACHE_LIBS_SHLIB@
libdpsearch_la_CFLAGS = @PTHREAD_CFLAGS@

else

mod_dpsearch_so_LDADD = libdpsearch.la libdpcharset.la @APACHE_LIBS_SHLIB@
libdpsearch_la_CFLAGS = @PTHREAD_CFLAGS@ -I@APACHE_INCDIR@ $(APACHE_CFLAGS)

endif


# Rules specific for Apache module
# $(LIBTOOL) --mode=link -c


if APACHE2

mod_dpsearch.so$(EXEEXT): $(mod_dpsearch_so_SOURCES) $(mod_dpsearch_so_DEPENDENCIES) 
	$(APXS) -c -o $@ $(INCLUDES) -Wc,"$(mod_dpsearch_so_CFLAGS)" -DAPACHE@APACHE_VER@ \
	     -DDPS_CONF_DIR=\\\"@sysconfdir@\\\" -DDPS_VAR_DIR=\\\"$(localstatedir)\\\" \
	     $(mod_dpsearch_so_SOURCES) $(mod_dpsearch_so_LDFLAGS) $(mod_dpsearch_so_LDADD); 

#install-exec-local: mod_dpsearch.so$(EXEEXT)
#	$(mkinstalldirs) $(DESTDIR)$(APACHE_MODDIR); 
#	if test -f .libs/mod_dpsearch.so$(EXEEXT); then \
#		$(APXS) -i -n dpsearch .libs/mod_dpsearch.so$(EXEEXT); \
#	else \
#		$(APXS) -i -n dpsearch mod_dpsearch.so$(EXEEXT); \
#	fi

else

mod_dpsearch.so$(EXEEXT): $(mod_dpsearch_so_SOURCES) $(mod_dpsearch_so_DEPENDENCIES) 
	$(LIBTOOL) --tag=CC --mode=link "$(APXS) -Wc,\"$(mod_dpsearch_so_CFLAGS)\"" -c -o $@ $(INCLUDES) -DAPACHE@APACHE_VER@ \
	     -DDPS_CONF_DIR=\\\"@sysconfdir@\\\" -DDPS_VAR_DIR=\\\"$(localstatedir)\\\" \
	     $(mod_dpsearch_so_SOURCES) $(mod_dpsearch_so_LDFLAGS) $(mod_dpsearch_so_LDADD); 

#install-exec-local: mod_dpsearch.so$(EXEEXT)
#	$(mkinstalldirs) $(DESTDIR)$(APACHE_MODDIR);
#	$(LIBTOOL) --mode=install $(INSTALL) mod_dpsearch.so $(DESTDIR)$(APACHE_MODDIR);

endif

#mod_dpsearch.so$(EXEEXT): $(mod_dpsearch_so_SOURCES) $(mod_dpsearch_so_DEPENDENCIES) 
#	if test "x$(APACHE_MODULE)" != "x"; then \
#	    $(LIBTOOL) --mode=link $(APXS) -c $(mod_dpsearch_so_SOURCES) -o $@ $(INCLUDES) \
#	     $(CFLAGS) $(mod_dpsearch_so_CFLAGS) $(mod_dpsearch_so_LDFLAGS) $(mod_dpsearch_so_LDADD) ; \
#	fi

#APACHE_MODDIR = @APACHE_MODDIR@
#	$(APXS) -i $(DESTDIR)$(sbindir)/mod_dpsearch.so$(EXEEXT)
#	if test -d .libs/.libs; then \
#		cp .libs/mod_* ./; \
#		mv .libs/.libs/* ./libs; \
#		rmdir .libs/.libs; \
#	fi 
#	if test -d .libs/.libs; then \
#		$(LIBTOOL) --mode=install $(INSTALL) .libs/.libs/mod_dpsearch.so$(EXEEXT) $(DESTDIR)$(APACHE_MODDIR); \
#	else \
#		$(LIBTOOL) --mode=install $(INSTALL) mod_dpsearch.so$(EXEEXT) $(DESTDIR)$(APACHE_MODDIR); \
#	fi 
##	if test "x$(APACHE_MODULE)" != "x"; then \
##		$(mkinstalldirs) $(DESTDIR)$(APACHE_MODDIR); \
##		$(INSTALL) mod_dpsearch.so \
##			$(DESTDIR)$(APACHE_MODDIR); \
##	fi
else

libdpsearch_la_CFLAGS = @PTHREAD_CFLAGS@
#install-exec-local:

endif



if PHPMODULE

dpsearch_so_SOURCES=php.c
dpsearch_so_CFLAGS=-fpic -DCOMPILE_DL=1 @PHP_INCLUDES@
dpsearch_so_LDFLAGS=-shared -L/usr/local/lib -rdynamic
dpsearch_so_LDADD=libdpsearch.la libdpcharset.la

endif

install-exec-local: @APACHE_MODULE@ @PHP_MODULE@
if APACHEMODULE
if APACHE2
	$(mkinstalldirs) $(DESTDIR)$(APACHE_MODDIR); 
	if test -f .libs/mod_dpsearch.so$(EXEEXT); then \
		$(APXS) -i -n dpsearch .libs/mod_dpsearch.so$(EXEEXT); \
	else \
		$(APXS) -i -n dpsearch mod_dpsearch.so$(EXEEXT); \
	fi
else
	$(mkinstalldirs) $(DESTDIR)$(APACHE_MODDIR);
	$(LIBTOOL) --mode=install $(INSTALL) mod_dpsearch.so $(DESTDIR)$(APACHE_MODDIR);
endif
endif
if PHPMODULE
	$(LIBTOOL) --mode=install $(INSTALL) dpsearch.so @PHP_EXTENSION_DIR@
endif


gcov_FILES = ${libdpsearch_la_SOURCES} ${indexer_SOURCES}

gcov-gen:
	list='${gcov_FILES}'; for p in $$list; do gcov $$p; done

gcov-clean:
	list='${gcov_FILES}'; for p in $$list; do rm -f $$p.gcov; done

INCLUDES = -I$(srcdir)/../include @INCLUDES@ \
@VIRT_INCLUDE@ \
@SOLID_INCLUDE@ \
@UNIXODBC_INCLUDE@ \
@DB2_INCLUDE@ \
@IODBC_INCLUDE@ \
@EASYSOFT_INCLUDE@ \
@MYSQL_INCLUDE@ \
@PGSQL_INCLUDE@ \
@MSQL_INCLUDE@ \
@IBASE_INCLUDE@ \
@CTLIB_INCLUDE@ \
@SQLITE_INCLUDE@ \
@SQLITE3_INCLUDE@ \
@ORACLE8_INCLUDE@ \
@ORACLE7_INCLUDE@ \
@SAPDB_INCLUDE@ \
@SSL_INCLUDE@ \
@DMALLOC_INCLUDE@ \
@BOEHMGC_INCLUDE@ \
@MECAB_INCLUDE@ 

LIBS = @LIBS@ @STATIC_LFLAGS@ \
@VIRT_LFLAGS@ \
@SOLID_LFLAGS@ \
@UNIXODBC_LFLAGS@ \
@DB2_LFLAGS@ \
@IODBC_LFLAGS@ \
@EASYSOFT_LFLAGS@ \
@MYSQL_LFLAGS@ \
@PGSQL_LFLAGS@ \
@MSQL_LFLAGS@ \
@IBASE_LFLAGS@ \
@CTLIB_LFLAGS@ \
@SQLITE_LFLAGS@ \
@SQLITE3_LFLAGS@ \
@ORACLE8_LFLAGS@ \
@ORACLE7_LFLAGS@ \
@SAPDB_LFLAGS@ \
@SSL_LFLAGS@ \
@DMALLOC_LFLAGS@ \
@BOEHMGC_LFLAGS@ \
@MECAB_LFLAGS@ 

