#ifndef CONFFILE_H
#define CONFFILE_H

#include <string>
#include <vector>
#include <set>
#include <stdexcept>
#include <cstring>
#include <cstdio>

class ConfFile {
public:
  ConfFile(const std::string &path);

  // Reload the configuration file.
  // If the new version is broken, the configuration is not modified.
  void reload();

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
    static std::string format(const ConfFile *cf,
                              const std::string &s);
  };

  // Configuration data
  std::set<std::string> files;          // files to watch
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
