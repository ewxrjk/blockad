#ifndef STDIOFILE_H
#define STDIOFILE_H

#include <string>
#include <cstdio>

// Wrapper for FILE *
//  - provides RAIIish cleanup
//  - reports IO errors as exceptions
//  - knows the path it was opened with
class StdioFile {
public:
  StdioFile();
  // Args as per fopen().
  StdioFile(const std::string &path, const std::string &mode = "r");
  // If fclose() fails in the destructor, the error is ignored.
  ~StdioFile();

  // Args as per fopen().  If this file was already open it is closed and any
  // errors ignored.
  void open(const std::string &path, const std::string &mode = "r");

  void close();                         // throws on error, unlike ~StdioFile()

  int readc();                          // return EOF or char
  bool readline(std::string &line);     // return true if a line, false at EOF
  
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
