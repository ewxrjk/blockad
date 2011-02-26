#include <config.h>
#include "IOError.h"
#include <cstring>

IOError::IOError(const std::string &what,
                 const std::string &path,
                 int errno_value):
  std::runtime_error(what + " " + path + ": " + strerror(errno_value)) {
}

IOError::IOError(const std::string &what,
                 int errno_value):
  std::runtime_error(what + ": " + strerror(errno_value)) {
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
