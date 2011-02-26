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
