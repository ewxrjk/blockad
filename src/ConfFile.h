#ifndef CONFFILE_H
#define CONFFILE_H

#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <cstring>
#include <cstdio>

#include "Regex.h"

class ConfFile {
public:
  ConfFile();
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
  private:
    static std::string format(const ConfFile *cf,
                              const std::string &s);
  };

  struct Match {
    Match(const Regex &r, int c): regex(r), capture(c) {}
    Regex regex;                        // regex to match
    int capture;                        // capture number
  };

  // Configuration data
  std::vector<std::string> files;       // files to watch
  std::vector<Match> patterns;          // patterns to match
  unsigned rate_max;                    // maximum occurences per interval
  unsigned rate_interval;               // interval size in seconds

  static const int rate_max_default = 5;
  static const int rate_interval_default = 60 * 60;
private:
  std::string path;
  int lineno;

  void parse();
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
