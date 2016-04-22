noinst_PROGRAMS = PTCChrome test

INCLUDES = -I. \
	-I$(top_srcdir)/lib \
	 `$(PTC_CONFIG) --cflags`

LIBS = \
	 `$(PTC_CONFIG) --libs`

PTCChrome_LDADD = $(top_builddir)/lib/libChrome.la

PTCChrome_SOURCES = \
	PTCmain.cpp \
	tstworld.c


test_LDADD = $(top_builddir)/lib/libChrome.la

test_SOURCES = \
	testmain.c

## dxmain.cpp
##	tstworld.c
