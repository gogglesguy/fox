#
#  Top-level makefile for IBM VisualAge C++ 3.5
#
all:
    cd ..\..\lib
    nmake -f Makefile.icc
    cd ..\utils
    nmake -f Makefile.icc
    cd ..\tests
    nmake -f Makefile.icc

clean:
    cd ..\..\lib
    nmake -f Makefile.icc clean
    cd ..\utils
    nmake -f Makefile.icc clean
    cd ..\tests
    nmake -f Makefile.icc clean
