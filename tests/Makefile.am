bin_PROGRAMS=watchfile regtest conftest
watchfile_SOURCES=watchfile.cc
regtest_SOURCES=regtest.cc
conftest_SOURCES=conftest.cc
LDADD=../src/libutils.a
AM_CPPFLAGS=-I${top_srcdir}/src
TESTS=t-watch regtest conftest
EXTRA_DIST=testconf1

clean-local:
	rm -f watch-results
	rm -f WatchMe
	rm -f DontWatchMe
