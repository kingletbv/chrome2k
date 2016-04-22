#
# Makefile.am - top-level makefile
#

SUBDIRS = lib example

mrproper : maintainer-clean
	rm -f core
	rm -f DEADJOE
	rm -f *~
	rm -f Makefile.in
	rm -f */core
	rm -f */DEADJOE
	rm -f */*~
	rm -f */Makefile.in
	rm -f */*/core
	rm -f */*/DEADJOE
	rm -f */*/*~
	rm -f */*/Makefile.in
	rm -f */*/*/core
	rm -f */*/*/DEADJOE
	rm -f */*/*/*~
	rm -f */*/*/Makefile.in
	rm -f config.*
	rm -f machine.*
	rm -f mkinstalldirs
	rm -f aclocal.m4
	rm -f missing
	rm -f ltmain.sh
	rm -f install-sh
	rm -f stamp-h*
	rm -f configure
	rm -f libtool
	rm -f ltconfig
	rm -rf autom4te.cache

tar: mrproper
	( cd .. && tar -zcf chrome.`date --iso-8601 +%y%m%d`.tar.gz chrome )

