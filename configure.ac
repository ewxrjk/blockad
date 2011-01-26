AC_PREREQ(2.61)
AC_INIT([banjammer], [0.0.WIP], [rjk@greenend.org.uk])
AC_CONFIG_AUX_DIR([config.aux])
AM_INIT_AUTOMAKE([banjammer], [0.0.WIP])
AC_CONFIG_SRCDIR([src/Watcher.h])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strerror])

# Enable GCC warnings
if test "x$GXX" = xyes; then
  CXX="$CXX -Wall -W -Werror -Wpointer-arith -Wwrite-strings"
fi

AC_OUTPUT([Makefile src/Makefile tests/Makefile])
