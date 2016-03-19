## Process this file with automake to produce Makefile.in

AM_CXXFLAGS = @X_CFLAGS@
INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include
RESWRAP  = $(top_builddir)/utils/reswrap$(EXEEXT)
SUFFIXES = .gif .bmp
LDADD = $(top_builddir)/lib/libFOX-1.7.la -lm


bin_PROGRAMS = calculator

man_MANS = calculator.1

ICONS  = \
colors.gif \
information.gif \
bigcalc.gif \
constmem.bmp \
question.gif \
tinycalc.gif


calculator_SOURCES = \
icons.h \
icons.cpp \
Calculator.h \
Calculator.cpp \
HelpWindow.h \
HelpWindow.cpp \
Preferences.h \
Preferences.cpp \
help.cpp \
main.cpp \
LICENSE \
$(ICONS)


Calculator.cpp: icons.h icons.cpp


icons.h: $(ICONS)
	$(RESWRAP) -h -o icons.h $(ICONS)


icons.cpp: $(ICONS) icons.h
	$(RESWRAP) -i icons.h -o icons.cpp $(ICONS)


CLEANFILES = icons.h icons.cpp


EXTRA_DIST = $(man_MANS) \
             Makefile.bc \
             Makefile.wc \
             Makefile.dmc

