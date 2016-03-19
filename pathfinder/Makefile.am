## Process this file with automake to produce Makefile.in

AM_CXXFLAGS = @X_CFLAGS@
INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include
RESWRAP  = $(top_builddir)/utils/reswrap$(EXEEXT)
SUFFIXES = .gif .bmp
LDADD	 = $(top_builddir)/lib/libFOX-1.7.la -lm

bin_PROGRAMS	= PathFinder

man_MANS = PathFinder.1

ICONS = \
addbook.bmp \
bigicons.bmp \
closepanel.gif \
clrbook.gif \
copy.bmp \
config_gif.gif \
copyit.gif \
cut.bmp \
delbook.bmp \
deleteit.bmp \
desktop.bmp \
details.bmp \
dirup.bmp \
enter.gif \
execute.bmp \
file_gif.gif \
foldernew.gif \
foxbig.gif \
foxmini.gif \
goback.bmp \
goforw.bmp \
gotodir.bmp \
home.gif \
hosts.bmp \
iconpath.gif \
linkit.bmp \
location.gif \
maphost.bmp \
miscellaneous_gif.gif \
moveit.gif \
paste.bmp \
pattern_gif.gif \
properties.bmp \
quit_gif.gif \
newfolder.bmp \
renameit.gif \
rotateleft.gif \
rotateright.gif \
run_bmp.bmp \
setbook.gif \
setdir.gif \
smallicons.bmp \
sorting.bmp \
unmaphost.bmp \
work.gif

PathFinder_SOURCES =  \
icons.h \
icons.cpp \
CopyDialog.h \
CopyDialog.cpp \
PathFinder.cpp \
PathFinder.h \
Preferences.cpp \
Preferences.h \
PropertyDialog.cpp \
PropertyDialog.h \
LICENSE \
$(ICONS)

PathFinder.cpp: icons.h icons.cpp

icons.h: $(ICONS)
	$(RESWRAP) -h -o icons.h $(ICONS)

icons.cpp: $(ICONS) icons.h
	$(RESWRAP) -i icons.h -o icons.cpp $(ICONS)

CLEANFILES	= icons.h icons.cpp

EXTRA_DIST = $(man_MANS) \
             Makefile.bc \
             Makefile.wc \
             Makefile.dmc

