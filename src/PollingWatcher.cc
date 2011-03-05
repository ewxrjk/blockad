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
#include <config.h>
#include "Watcher.h"
#include "PollingWatcher.h"
#include "IOError.h"
#include <cerrno>
#include <sys/stat.h>

PollingWatcher::PollingWatcher(const std::string &path,
                               Watcher *watcher):
  WatcherImplementation(path, watcher),
  next(time(NULL)),
  dev(-1),
  ino(-1) {
  // Try opening the file
  if((fp = fopen(path.c_str(), "r"))) {
    if(fseek(fp, 0, SEEK_END) < 0)
      throw IOError("seeking " + path, errno);
  }
}

PollingWatcher::~PollingWatcher() {
  if(fp)
    fclose(fp);
}

int PollingWatcher::pollfd(time_t &limit) const {
  if(limit > next)
    limit = next;
  return -1;
}

void PollingWatcher::openFile() {
  struct stat sb;
  if(!fp) {
    if(!(fp = fopen(path.c_str(), "r"))) {
      if(errno != ENOENT)
        throw IOError("opening " + path, errno);
      return;
    }
    if(fstat(fileno(fp), &sb) < 0)
      throw IOError("fstat " + path, errno);
    dev = sb.st_dev;
    ino = sb.st_ino;
  }
}

void PollingWatcher::closeFile() {
  if(fp) {
    if(line.size()) {
      processLine(line);
      line.clear();
    }
    fclose(fp);
    fp = NULL;
    dev = -1;
    ino = -1;
  }
}

void PollingWatcher::work() {
  // Check again in a second
  next = time(NULL) + 1;
  // If the file is open, verify that we're still reading the right one
  if(fp) {
    struct stat sb;
    if(stat(path.c_str(), &sb) < 0
       || sb.st_ino != ino
       || sb.st_dev != dev)
      closeFile();
  }
  // If the file wasn't open, see if it is now
  if(!fp) {
    openFile();
    if(!fp)
      return;
  }
  // The file must now be open
  readLines();
}

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
