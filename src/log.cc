//
// Copyright © 2011 Richard Kettlewell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <config.h>
#include "log.h"
#include <cstdarg>
#include <cstdio>
#include <syslog.h>

bool debugging;

static void stderrlog(int pri, const char *fmt, va_list ap) {
  static const char *const pritab[] = {
    "EMERG",
    "ALERT",
    "CRIT",
    "ERROR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG",
  };
  fprintf(stderr, "%s: ", pritab[pri]);
  vfprintf(stderr, fmt, ap);
  fputc('\n', stderr);
}

static void (*vlog)(int, const char *, va_list) = stderrlog;

void debug_(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_DEBUG, format, ap);
  va_end(ap);
}

void info(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_INFO, format, ap);
  va_end(ap);
}

void error(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vlog(LOG_ERR, format, ap);
  va_end(ap);
}

void useSyslog(const char *ident) {
  openlog(ident, LOG_PID, LOG_DAEMON);
  vlog = vsyslog;
}

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
