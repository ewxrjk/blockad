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
#ifndef CONFFILE_H
#define CONFFILE_H

#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <cstring>
#include <cstdio>

#include "Regex.h"
#include "Address.h"

class BlockMethod;

class ConfFile {
public:
  // Read the config file
  ConfFile(const std::string &path);

  // Base class for configuration file errors
  class Error: public std::runtime_error {
  public:
    Error(const std::string &s): std::runtime_error(s) {}
  };
  
  // Syntax error in configuration file
  class SyntaxError: public Error {
  public:
    SyntaxError(const ConfFile *cf,
                const std::string &s): Error(format(cf, s)) {}
    SyntaxError(const ConfFile *cf,
                const std::runtime_error &e): Error(format(cf, e.what())) {}
  private:
    static std::string format(const ConfFile *cf,
                              const std::string &s);
  };

  // A pattern to match
  struct Match {
    Match(const Regex &r, int c): regex(r), capture(c) {}
    Regex regex;                        // regex to match
    int capture;                        // capture number
  };

  // Configuration data
  std::vector<std::string> files;       // files to watch
  std::vector<Match> patterns;          // patterns to match
  std::vector<AddressPattern> exempted; // never ban these addresses
  unsigned rate_max;                    // maximum occurences per interval
  unsigned rate_interval;               // interval size in seconds
  const BlockMethod *block;             // block method

  // Default settings
  static const int rate_max_default = 5;
  static const int rate_interval_default = 60 * 60;
  static const char block_default[];
private:
  std::string path;                     // file that's being read
  int lineno;                           // current line number

  void parse();
  void parseLine(const std::string &line);
  size_t splitLine(const std::string &line,
                   std::vector<std::string> &bits);
  size_t parseString(const std::string &line,
                     size_t pos,
                     std::string &bit,
                     char q);
  static int decodeHex(char c);
};

#endif /* CONFFILE_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
