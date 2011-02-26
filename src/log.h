#ifndef LOG_H
#define LOG_H

#define debug(...) do {                         \
  if(debugging)                                 \
    debug_(__VA_ARGS__);                        \
} while(0)
  

void debug_(const char *format, ...);
void info(const char *format, ...);
void error(const char *format, ...);

void useSyslog(const char *ident);

extern bool debugging;

#endif /* LOG_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
