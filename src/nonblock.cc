#include <config.h>
#include "utils.h"
#include "IOError.h"
#include <cerrno>
#include <fcntl.h>

void nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if(flags < 0)
    throw IOError("fcntl F_GETFL", errno);
  if(!(flags & O_NONBLOCK))
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
      throw IOError("fcntl F_SETFL", errno);
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
