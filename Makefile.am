## Process this file with automake to produce Makefile.in

# For non-gcc compilers, change the options to "no-dependencies"
AUTOMAKE_OPTIONS = foreign dist-zip

SUBDIRS    = utils include lib chart doc tests shutterbug adie pathfinder calculator controlpanel windows
EXTRA_DIST = ADDITIONS AUTHORS INSTALL LICENSE LICENSE_ADDENDUM TRACING README fox.spec index.html aclocal.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = fox17.pc

CLEANFILES = $(distdir).tar.gz $(distdir).zip

snapshot: dist-all
	cp $(distdir).tar.gz /net/webby/ftp/pub/fox-snapshot.tar.gz
	cp $(distdir).zip /net/webby/ftp/pub/fox-snapshot.zip



web: distdir
	cd $(distdir)/doc && doxygen doxygen.cfg
	cp -r $(distdir)/doc/* /net/webby/html

#web: distdir
#	cp -r $(distdir)/doc/* /net/webby/html

drop: dist-all
	cp $(distdir).tar.gz /net/webby/ftp/pub
	cp $(distdir).zip /net/webby/ftp/pub
	cd /net/webby/ftp/pub && ln -sf $(distdir).tar.gz fox-latest.tar.gz
	cd /net/webby/ftp/pub && ln -sf $(distdir).zip fox-latest.zip


#diff: distdir
#	-diff -P -c -r fox-1.1.8 $(distdir) > fox-patch-1.1.8-$(distdir)

# Suggestion for the above
# cvs rdiff -u -kk -r fox-1.0.8 -r fox-1.0.9 FOX > fox-path-1.0.8-1.0.9


#dist-hook:
#	cp fox.spec $(distdir)
#	cp -dpR windows $(distdir)/windows
#	-find $(distdir)/windows -name CVS -exec rm -rf {} \;



