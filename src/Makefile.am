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
noinst_LIBRARIES=libutils.a
sbin_PROGRAMS=blockad
man_MANS=blockad.8
libutils_a_SOURCES=Address.cc ConfFile.cc Regex.cc StdioFile.cc		\
Watcher.cc Address.h ConfFile.h Regex.h StdioFile.h Watcher.h log.h	\
log.cc IOError.cc IOError.h nonblock.cc utils.h execute.cc		\
BlockMethod.h BlockMethod.cc WatcherImplementation.h			\
WatcherImplementation.cc InotifyWatcher.cc InotifyWatcher.h		\
PollingWatcher.cc PollingWatcher.h timeval.h
blockad_SOURCES=blockad.cc iptables.cc hosts.deny.cc
LDADD=libutils.a
EXTRA_DIST=${man_MANS}
