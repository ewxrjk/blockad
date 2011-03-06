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
#ifndef TIMEVAL_H
#define TIMEVAL_H

#include <sys/time.h>
#include <limits.h>

#if SIZEOF_TIME_T == SIZEOF_INT
# define TIME_MAX INT_MAX
#elif SIZEOF_TIME_T == SIZEOF_LONG
# define TIME_MAX LONG_MAX
#elif SIZEOF_TIME_T == SIZEOF_LONG_LONG
# define TIME_MAX LLONG_MAX
#else
# error cannot figure out TIME_MAX
#endif

inline struct timeval operator-(const struct timeval &a,
				const struct timeval &b) {
  struct timeval r;
  r.tv_sec = a.tv_sec - b.tv_sec;
  r.tv_usec = a.tv_usec - b.tv_usec;
  if(r.tv_usec < 0) {
    r.tv_usec += 1000000;
    r.tv_sec -= 1;
  }
  return r;
}

inline struct timeval operator-(time_t a,
				const struct timeval &b) {
  struct timeval aa;
  aa.tv_sec = a;
  aa.tv_usec = 0;
  return aa - b;
}

inline bool operator<(const struct timeval &a,
		      const struct timeval &b) {
  if(a.tv_sec < b.tv_sec)
    return true;
  if(a.tv_sec == b.tv_sec && a.tv_usec < b.tv_usec)
    return true;
  return false;
}

inline bool operator<(const struct timeval &a,
		      time_t b) {
  if(a.tv_sec < b)
    return true;
  return false;
}

inline bool operator>=(const struct timeval &a,
		      time_t b) {
  if(a.tv_sec >= b)
    return true;
  return false;
}

#endif /* TIMEVAL_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
