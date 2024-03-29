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
#ifndef IOERROR_H
#define IOERROR_H

#include <stdexcept>
#include <string>

// Thrown if any IO error occurs
class IOError: public std::runtime_error {
public:
  IOError(const std::string &what, const std::string &path, int errno_value);
  IOError(const std::string &what, int errno_value);
};

// Thrown if any system error occurs
class SystemError: public std::runtime_error {
public:
  SystemError(const std::string &what, int errno_value);
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
