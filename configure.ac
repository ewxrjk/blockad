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
AC_PREREQ([2.61])
AC_INIT([blockad], [0.0.DEV], [rjk@greenend.org.uk])
AC_CONFIG_AUX_DIR([config.aux])
AM_INIT_AUTOMAKE([1.10])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES])
AC_CONFIG_SRCDIR([src/Watcher.h])
AC_CONFIG_HEADER([config.h])

# Identify platform
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_LANG([C++])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([sys/inotify.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([time_t])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])

# Checks for library functions.
AC_CHECK_FUNCS([strerror])

# Enable GCC warnings
if test "x$GXX" = xyes; then
  CXX="$CXX -Wall -W -Werror -Wpointer-arith -Wwrite-strings"
  AS_CASE([$host],
    [*-apple-darwin*], [CXX="$CXX -Wno-deprecated-declarations"])
fi

AC_OUTPUT([Makefile src/Makefile tests/Makefile])
