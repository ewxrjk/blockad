#ifndef IOERROR_H
#define IOERROR_H

#include <stdexcept>
#include <string>

// Thrown if any IO error occurs
class IOError: public std::runtime_error {
public:
  IOError(const std::string &what,
          const std::string &path,
          int errno_value);
  IOError(const std::string &what,
          int errno_value);
};

#endif /* IOERROR_H */


/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
