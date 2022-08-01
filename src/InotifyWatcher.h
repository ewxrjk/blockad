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
#ifndef INOTIFYWATCHER_H
#define INOTIFYWATCHER_H

#if HAVE_SYS_INOTIFY_H
#include "Watcher.h"

class InotifyWatcher: public WatcherImplementation {
public:
  InotifyWatcher(const std::string &path, Watcher *watcher);
  ~InotifyWatcher();
  int pollfd(time_t &limit) const;
  void work();

private:
  const std::string base; // base filename
  const std::string dir;  // containing directory
  int ifd;                // inotify descriptor
  int file_wd;            // file watch descriptor or -1
  int dir_wd;             // directory watch descriptor

  void openFile();  // try to ensure the watched file open
  void closeFile(); // close the watched file if open
};
#endif

#endif /* INOTIFYWATCHER_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
