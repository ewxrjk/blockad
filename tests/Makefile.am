#
# Copyright © 2011 Richard Kettlewell
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
noinst_PROGRAMS=watchfile regtest conftest addrtest
watchfile_SOURCES=watchfile.cc
regtest_SOURCES=regtest.cc
conftest_SOURCES=conftest.cc
addrtest_SOURCES=addrtest.cc
LDADD=../src/libutils.a
AM_CPPFLAGS=-I${top_srcdir}/src
TESTS=t-watch regtest conftest addrtest
EXTRA_DIST=testconf1 watch-expect

clean-local:
	rm -f watch-results
	rm -f WatchMe
	rm -f DontWatchMe
