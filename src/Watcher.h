#ifndef WATCHER_H
#define WATCHER_H

#include <string>
#include <stdexcept>
#include <cstring>

// Watch a filename and supply lines written to it to a callback.
class Watcher {
public:
  // Create a watcher for PATH
  Watcher(const std::string &path);
  ~Watcher();

  // Return the file descriptor to poll
  inline int pollfd() const { return ifd; }

  // Do some work.  Call this when pollfd() polls readable, or just
  // repeatedly if you didn't make it nonblocking.
  void work();

  // Passed a new line from the watched file.  line includes the trailing '\n',
  // if there was one; there may not be if a file is finished off without a
  // newline.
  virtual void processLine(const std::string &line) = 0;

  // Base class for Watcher errors
  class Error: public std::runtime_error {
  public:
    Error(const std::string &s, int e = 0): 
      std::runtime_error(e ? s + ": " + strerror(e)
                           : s),
      errno_value(e) {
    }

    // The errno value associated with the error, or 0.
    const int errno_value;
  };

  // Exception thrown if something goes wrong with the watched file.  You
  // should be able to still use the Watcher, in principle, though it's likely
  // that the file being watched is on a broken disk or something similar, so
  // it may not be very useful.
  class IOError: public Error {
  public:
    IOError(const std::string &s, int e = 0): Error(s, e) { }
  };

  // Exception thrown if something goes wrong with the infrastructure.  After
  // SystemError is thrown, the Watcher is probably broken beyond repair.
  class SystemError: public Error {
  public:
    SystemError(const std::string &s, int e = 0): Error(s, e) { }
  };

private:
  const std::string path;               // filename to watch
  const std::string base;               // base filename
  const std::string dir;                // containing directory
  std::string line;                     // line read so far
  FILE *fp;                             // (may be NULL) current file
  int ifd;                              // inotify descriptor
  int file_wd;                          // file watch descriptor or -1
  int dir_wd;                           // directory watch descriptor

  void openFile();                      // try to ensure the watched file open
  void closeFile();                     // close the watched file if open
  void readLines();                     // read lines from the watch file
  static std::string getBaseName(const std::string &); // get base name
  static std::string getDirName(const std::string &); // get directory name
};

#endif /* WATCHER_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
