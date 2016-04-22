lib_LTLIBRARIES = libChrome.la
include_HEADERS = $(libChrome_headers)

INCLUDES = -I.

LIBS = 

libChrome_headers = \
	actor.h \
	actptset.h \
	colormgr.h \
	edgetbl.h \
	floatset.h \
	frame.h \
	hplane.h \
	indexset.h \
	lmap1.h \
	lmap256.h \
	model.h \
	nffmodel.h \
	octree.h \
	parsebuf.h \
	plane.h \
	planeset.h \
	pmodel.h \
	polygon.h \
	polyset.h \
	scrvertx.h \
	scvtxset.h \
	texmap.h \
	trans.h \
	vector.h \
	vertex.h \
	vertxset.h \
	vpoint.h

libChrome_la_SOURCES = \
	actor.c \
	actptset.c \
	colormgr.c \
	edgetbl.c \
	floatset.c \
	frame.c \
	hplane.c \
	indexset.c \
	lmap256.c\
	model.c \
	nffmodel.c \
	octree.c \
	parsebuf.c \
	plane.c \
	planeset.c \
	pmodel.c \
	polygon.c \
	polyset.c \
	scvtxset.c \
	texmap.c \
	trans.c \
	vertex.c \
	vertxset.c \
	vpoint.c \
	$(libChrome_headers)

libChrome_la_LDFLAGS = -version-info @CHROME_CURRENT@:@CHROME_REVISION@:@CHROME_AGE@ -release @CHROME_RELEASE@

