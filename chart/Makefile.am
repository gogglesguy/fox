## Process this file with automake to produce Makefile.in

CXXFLAGS = @CXXFLAGS@ @X_CFLAGS@
INCLUDES = -I$(top_srcdir)/include -I$(top_builddir)/include
RESWRAP  = $(top_builddir)/utils/reswrap
SUFFIXES = .gif .bmp
LDADD	 =  libCHART-1.7.la $(top_builddir)/src/libFOX-1.7.la -lm

lib_LTLIBRARIES = libCHART-1.7.la

libCHART_1_7_la_LIBADD  = $(X_LIBS) $(X_BASE_LIBS) $(X_EXTRA_LIBS)

libCHART_1_7_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -export-dynamic

chartincludedir=$(includedir)/fox-1.7/chart

chartinclude_HEADERS = \
FX2DChart.h \
FX2DPlot.h \
FXAxis.h \
FXChart.h \
FXCurve.h \
chartdefs.h \
chart.h

libCHART_1_7_la_SOURCES = \
FX2DChart.cpp \
FX2DPlot.cpp \
FXAxis.cpp \
FXChart.cpp \
FXCurve.cpp \
chartutils.h \
chartutils.cpp


noinst_PROGRAMS = charttest

ICONS = marble.bmp

charttest_SOURCES = charttest.cpp icons.h icons.cpp $(ICONS)

charttest.cpp: icons.h icons.cpp

icons.h: $(ICONS)
	$(RESWRAP) -h -o icons.h $(ICONS)

icons.cpp: $(ICONS) icons.h
	$(RESWRAP) -i icons.h -o icons.cpp $(ICONS)

CLEANFILES = icons.h icons.cpp


EXTRA_DIST = Makefile.bc
