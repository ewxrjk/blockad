//
// Copyright © 2011, 2012 Richard Kettlewell
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

#if HAVE_SYS_INOTIFY_H

#include "Watcher.h"
#include "InotifyWatcher.h"
#include <sys/inotify.h>
#include <cerrno>
#include <cstdio>
#include <unistd.h>
#include "log.h"

InotifyWatcher::InotifyWatcher(const std::string &path_arg, Watcher *watcher):
    WatcherImplementation(path_arg, watcher), base(getBaseName(path)),
    dir(getDirName(path)), ifd(-1), file_wd(-1), dir_wd(-1) {
  // Get an inotify FD
  if((ifd = inotify_init()) < 0)
    throw Watcher::SystemError("inotify_init", errno);
  // Watch the containing directory
  if((dir_wd = inotify_add_watch(ifd, dir.c_str(),
                                 IN_CREATE | IN_DELETE | IN_MOVED_FROM
                                     | IN_MOVED_TO))
     < 0)
    throw Watcher::SystemError("inotify_add_watch for " + dir, errno);
  // Try to open the file.  This will add file_wd if it exists.
  openFile();
  // Discard initial content
  if(fp) {
    if(fseek(fp, 0, SEEK_END) < 0)
      throw Watcher::IOError("seeking " + path, errno);
  }
}

InotifyWatcher::~InotifyWatcher() {
  if(ifd >= 0)
    close(ifd);
}

int InotifyWatcher::pollfd(time_t &) const {
  return ifd;
}

void InotifyWatcher::openFile() {
  if(!fp) {
    // Try to open the file.  If it doesn't exist, that's OK, we'll continue to
    // wait for it to come into existence.
    if(!(fp = fopen(path.c_str(), "r"))) {
      if(errno != ENOENT)
        throw Watcher::IOError("opening " + path, errno);
    } else {
      // If we did open it then detect changes to it.
      // (But see below concerning IN_*_SELF.)
      if((file_wd = inotify_add_watch(
              ifd, path.c_str(), IN_MODIFY | IN_MOVE_SELF | IN_DELETE_SELF))
         < 0)
        throw Watcher::SystemError("inotify_add_watch for " + path, errno);
    }
  }
}

void InotifyWatcher::closeFile() {
  if(fp) {
    if(line.size()) {
      processLine(line);
      line.clear();
    }
    if(file_wd >= 0) {
      if(inotify_rm_watch(ifd, file_wd) < 0) {
        file_wd = -1;
        // On 2.6.26 we get a spurious EINVAL removing a wd sometimes.  It
        // doesn't happen on 2.6.32.  We ignore it and issue a warning instead.
        // There's not much else that can be done.
        if(errno == EINVAL)
          warn("inotify_rm_watch: %s", strerror(errno));
        else
          throw Watcher::SystemError("inotify_rm_watch for " + path, errno);
      }
      file_wd = -1;
    }
    fclose(fp);
    fp = NULL;
  }
}

void InotifyWatcher::work() {
  char buffer[4096];
  int n = read(ifd, buffer, sizeof buffer);
  if(n < 0) {
    if(errno != EINTR && errno != EAGAIN)
      throw Watcher::SystemError("read from inotify fd", errno);
    return;
  }
  // We might have more than one event.
  char *ptr = buffer;
  while(n > 0) {
    // Make sure we have a whole event
    if((size_t)n < sizeof(struct inotify_event))
      throw Watcher::SystemError("short read from inotify fd");
    const struct inotify_event &event = *(const struct inotify_event *)ptr;
    const size_t total = sizeof event + event.len;
    if((size_t)n < total)
      throw Watcher::SystemError("short read from inotify fd");

    // See if the file contents changed.
    if(event.wd == file_wd) {
      // Read as much as we can from the file, whatever happened
      readLines();
      // If the file was deleted or renamed away, we're done with this version
      // of it.  Note that we don't seem to get (at least) IN_DELETE_SELF
      // reliably!  However we handle the cases that ought to produce it below
      // in the dir_wd checking too.
      if(event.mask & (IN_MOVE_SELF | IN_DELETE_SELF)) {
        // Close the file.
        closeFile();
      }
    }

    if(event.wd == dir_wd && event.len && !strcmp(event.name, base.c_str())) {
      // The file has been newly created or replaced by another file.  Close
      // the old version (if open).
      closeFile();
      // Open the new version (if it exists).
      openFile();
      if(fp) {
        // Something might have been written to it since it was created but
        // before we started watching it, so we check straight away.
        readLines();
      }
    }
    ptr += total;
    n -= total;
  }
}

#endif

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
