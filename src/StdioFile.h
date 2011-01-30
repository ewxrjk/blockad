#ifndef STDIOFILE_H
#define STDIOFILE_H

#include <string>
#include <stdexcept>
#include <cstdio>

// Wrapper for FILE *
class StdioFile {
public:
  StdioFile();
  StdioFile(const std::string &path, const std::string &mode = "r");
  ~StdioFile();

  void open(const std::string &path, const std::string &mode = "r");
  void close();

  int readc();                          // return EOF or char
  bool readline(std::string &line);     // return true if a line, false at EOF
  
  class IOError: public std::runtime_error {
  public:
    IOError(const std::string &what,
            const std::string &path,
            int errno_value);
  };
private:
  std::string path;
  FILE *fp;
};

#endif /* STDIOFILE_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
