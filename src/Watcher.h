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
#ifndef WATCHER_H
#define WATCHER_H

#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdio>

#if HAVE_SYS_INOTIFY_H
#define WATCHER InotifyWatcher
#else
#define WATCHER PollingWatcher
#endif

class Watcher;

// Base class for watcher implementations
class WatcherImplementation {
public:
  WatcherImplementation(const std::string &path_, Watcher *watcher_):
      path(path_), fp(NULL), watcher(watcher_) {}
  virtual ~WatcherImplementation();
  virtual int pollfd(time_t &limit) const = 0;
  virtual void work() = 0;
  inline void processLine(const std::string &line);

protected:
  const std::string path;
  FILE *fp;
  std::string line; // line read so far

  virtual void closeFile() = 0;

  static std::string getBaseName(const std::string &); // get base name
  static std::string getDirName(const std::string &);  // get directory name

  void readLines(); // read lines from the watch file
private:
  Watcher *watcher;
};

// Watch a filename and supply lines written to it to a callback.
class Watcher {
public:
  // Create a watcher for PATH
  Watcher(const std::string &path);
  virtual ~Watcher();

  // Return the file descriptor to poll
  int pollfd(time_t &limit) const {
    return impl->pollfd(limit);
  }

  // Do some work.  Call this when pollfd() polls readable, or just
  // repeatedly if you didn't make it nonblocking.
  void work() {
    impl->work();
  }

  // Passed a new line from the watched file.  line includes the trailing '\n',
  // if there was one; there may not be if a file is finished off without a
  // newline.
  virtual void processLine(const std::string &line) = 0;

  // Base class for Watcher errors
  class Error: public std::runtime_error {
  public:
    Error(const std::string &s, int e = 0):
        std::runtime_error(e ? s + ": " + strerror(e) : s), errno_value(e) {}

    // The errno value associated with the error, or 0.
    const int errno_value;
  };

  // Exception thrown if something goes wrong with the watched file.  You
  // should be able to still use the Watcher, in principle, though it's likely
  // that the file being watched is on a broken disk or something similar, so
  // it may not be very useful.
  class IOError: public Error {
  public:
    IOError(const std::string &s, int e = 0): Error(s, e) {}
  };

  // Exception thrown if something goes wrong with the infrastructure.  After
  // SystemError is thrown, the Watcher is probably broken beyond repair.
  class SystemError: public Error {
  public:
    SystemError(const std::string &s, int e = 0): Error(s, e) {}
  };

private:
  WatcherImplementation *impl;
};

inline void WatcherImplementation::processLine(const std::string &line) {
  watcher->processLine(line);
}

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
