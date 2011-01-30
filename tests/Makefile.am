bin_PROGRAMS=watchfile regtest
watchfile_SOURCES=watchfile.cc
regtest_SOURCES=regtest.cc
LDADD=../src/libutils.a
AM_CPPFLAGS=-I${top_srcdir}/src
TESTS=t-watch regtest

clean-local:
	rm -f watch-results
	rm -f WatchMe
	rm -f DontWatchMe
