#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="Chrome"

(test -f $srcdir/configure.in \
  && test -d $srcdir/lib \
  && test -f $srcdir/lib/scvtxset.c) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level Chrome 3D engine directory"
    exit 1
}

. $srcdir/macros/autogen.sh
