## Process this file with automake to produce Makefile.in 
AUTOMAKE_OPTIONS = foreign no-installinfo dist-bzip2 no-dist-gzip
SUBDIRS = include src doc etc create misc scripts test #php @BINDINGS@
#DIST_SUBDIRS = php java perl

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/splitter/
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/tree/
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/cache/
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/store/
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/url/
	chmod 777 $(DESTDIR)$(localstatedir)/cache/

uninstall-local:
	-rmdir $(DESTDIR)$(localstatedir)/splitter/
	-rmdir $(DESTDIR)$(localstatedir)/tree/
	-rmdir $(DESTDIR)$(localstatedir)/cache/
	-rmdir $(DESTDIR)$(localstatedir)/store/
	-rmdir $(DESTDIR)$(localstatedir)/url/
	-rmdir $(DESTDIR)$(datadir)
	-rmdir $(DESTDIR)$(sysconfdir)
	-rmdir $(DESTDIR)$(mandir)/man1
	-rmdir $(DESTDIR)$(mandir)/man5
	-rmdir $(DESTDIR)$(mandir)
	-rmdir $(DESTDIR)$(prefix)/doc
	-rmdir $(DESTDIR)$(localstatedir)
	-rmdir $(DESTDIR)$(bindir)
	-rmdir $(DESTDIR)$(sbindir)
	-rmdir $(DESTDIR)$(libdir)
	-rmdir $(DESTDIR)$(includedir)
	-rmdir $(DESTDIR)$(datadir)
	-rmdir $(DESTDIR)$(prefix)

bin-dist:
	$(MAKE) install DESTDIR=$(CDIR)/$(PACKAGE)-$(VERSION)
	cd $(CDIR)/$(PACKAGE)-$(VERSION)$(prefix) \
		&& tar -ycPf $(PACKAGE)-$(VERSION)-$(DBTYPE)-$(host_triplet)-$(LINKING).tar.bz2 ./
	mv $(CDIR)/$(PACKAGE)-$(VERSION)$(prefix)/*.tar.bz2 $(CDIR)
	rm -r $(CDIR)/$(PACKAGE)-$(VERSION)

EXTRA_DIST = net.m4 threads.m4 install.pl bootstrap acinclude.m4 docbook.m4 README.XML BUGS \
	perl/Makefile.PL.in perl/BUGS perl/README perl/INSTALL perl/test.pl perl/Dataparksearch.pm perl/Dataparksearch.xs \
	perl/cat_ed/*.pl perl/cat_ed/*.gif perl/examples/*


libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

## libdpcharset distribution

CHARSETDISTFILES =  src/uconv.c src/unidata.c src/sgml.c src/unicode.c src/dpconv.c \
src/uconv-8bit.c src/uconv-alias.c src/uconv-big5.c src/uconv-big5hkscs.c src/uconv-eucjp.c src/uconv-euckr.c \
src/uconv-gb2312.c src/uconv-gbk.c src/uconv-sjis.c src/uconv-sys.c src/uconv-utf8.c src/uconv-gujarati.c \
src/uconv-tscii.c src/charset-utils.c src/xmalloc.c src/sgml.entities src/sgml.inc src/uconv-8bit.inc src/unidata.ch \
include/dps_config.h.in include/dps_uniconv.h include/dps_sgml.h include/dps_unicode.h \
include/dps_unidata.h src/uconv-8bit.h include/dps_charsetutils.h include/dps_filence.h include/dps_xmalloc.h \
COPYING

charsetdistdir = libdpcharset-$(VERSION)

charsetdistdir: $(CHARSETDISTFILES)
	-chmod -R a+w $(charsetdistdir) >/dev/null 2>&1; rm -rf $(charsetdistdir)
	mkdir $(charsetdistdir)
	$(mkinstalldirs) $(charsetdistdir)/. $(charsetdistdir)/././include $(charsetdistdir)/./include $(charsetdistdir)/src
	@for file in $(CHARSETDISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    $(mkinstalldirs) "$(charsetdistdir)/$$dir"; \
	  fi; \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(charsetdistdir) \
	    || exit 1; \
	  else \
	    test -f $(charsetdistdir)/$$file \
	    || cp -p $$d/$$file $(charsetdistdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	cp bootstrap $(charsetdistdir)
	cp charset-configure.in $(charsetdistdir)/configure.in
	cp charset-Makefile.am $(charsetdistdir)/Makefile.am
	cp src/charset-Makefile.am $(charsetdistdir)/src/Makefile.am
	-find $(charsetdistdir) -type d ! -perm -777 -exec chmod a+rwx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(SHELL) $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r $(charsetdistdir)


charset-dist: charsetdistdir
	$(AMTAR) chof - $(charsetdistdir) | GZIP=$(GZIP_ENV) gzip -c >$(charsetdistdir).tar.gz
	-chmod -R a+w $(charsetdistdir) >/dev/null 2>&1; rm -rf $(charsetdistdir)


dist-hook:
	cp -rf $(srcdir)/debian $(distdir)

