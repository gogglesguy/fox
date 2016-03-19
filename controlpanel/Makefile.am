## Process this file with automake to produce Makefile.in

CXXFLAGS = @CXXFLAGS@ @X_CFLAGS@
INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include
RESWRAP  = $(top_builddir)/utils/reswrap
SUFFIXES = .gif .bmp
LDADD = $(top_builddir)/src/libFOX-1.7.la -lm


bin_PROGRAMS = ControlPanel

man_MANS = ControlPanel.1

ICONS  = \
controlpanel_gif.gif \
colors_gif.gif \
filebinding_gif.gif \
settings_gif.gif

ControlPanel_SOURCES = \
icons.h \
icons.cpp \
ControlPanel.cpp \
LICENSE \
$(ICONS)


ControlPanel.cpp: icons.h icons.cpp


icons.h: $(ICONS)
	$(RESWRAP) -h -o icons.h $(ICONS)


icons.cpp: $(ICONS) icons.h
	$(RESWRAP) -i icons.h -o icons.cpp $(ICONS)


CLEANFILES = icons.h icons.cpp


EXTRA_DIST = $(man_MANS) \
             Makefile.bc \
             Makefile.wc \
             Makefile.dmc

