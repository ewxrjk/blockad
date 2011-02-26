#include <config.h>
#include "Watcher.h"
#include <sys/inotify.h>
#include <cerrno>
#include <cstdio>

Watcher::Watcher(const std::string &path_arg): path(path_arg),
                                               base(getBaseName(path)),
                                               dir(getDirName(path)),
                                               fp(NULL),
                                               ifd(-1),
                                               file_wd(-1),
                                               dir_wd(-1) {
  // Get an inotify FD
  if((ifd = inotify_init()) < 0)
    throw SystemError("inotify_init", errno);
  // Watch the containing directory
  if((dir_wd = inotify_add_watch(ifd, dir.c_str(),
                                 IN_CREATE|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO)) < 0)
    throw SystemError("inotify_add_watch for " + dir, errno);
  // Try to open the file.  This will add file_wd if it exists.
  openFile();
  // Discard initial content
  if(fp) {
    if(fseek(fp, 0, SEEK_END) < 0)
      throw IOError("seeking " + path, errno);
  }
}

Watcher::~Watcher() {
  if(ifd >= 0)
    close(ifd);
  if(fp)
    fclose(fp);
}

void Watcher::openFile() {
  if(!fp) {
    // Try to open the file.  If it doesn't exist, that's OK, we'll continue to
    // wait for it to come into existence.
    if(!(fp = fopen(path.c_str(), "r"))) {
      if(errno != ENOENT)
        throw IOError("opening " + path, errno);
    } else {
      // If we did open it then detect changes to it.
      // (But see below concerning IN_*_SELF.)
      if((file_wd = inotify_add_watch(ifd, path.c_str(),
                                      IN_MODIFY
                                      |IN_MOVE_SELF
                                      |IN_DELETE_SELF)) < 0)
        throw SystemError("inotify_add_watch for " + path, errno);
    }
  }
}

void Watcher::closeFile() {
  if(fp) {
    if(line.size()) {
      processLine(line);
      line.clear();
    }
    if(file_wd >= 0) {
      if(inotify_rm_watch(ifd, file_wd) < 0) {
        file_wd = -1;
        throw SystemError("inotify_rm_watch for " + path, errno);
      }
      file_wd = -1;
    }
    fclose(fp);
    fp = NULL;
  }
}

std::string Watcher::getDirName(const std::string &p) {
  std::string::size_type s = p.rfind('/');
  if(s == std::string::npos)
    return ".";
  else
    return std::string(p, 0, s + 1);
}

std::string Watcher::getBaseName(const std::string &p) {
  std::string::size_type s = p.rfind('/');
  if(s == std::string::npos)
    return p;
  else
    return std::string(p, s + 1);
}

void Watcher::work() {
  char buffer[4096];
  int n = read(ifd, buffer, sizeof buffer);
  if(n < 0) {
    if(errno != EINTR && errno != EAGAIN)
      throw SystemError("read from inotify fd", errno);
    return;
  }
  // We might have more than one event.
  char *ptr = buffer;
  while(n > 0) {
    // Make sure we have a whole event
    if((size_t)n < sizeof (struct inotify_event))
      throw SystemError("short read from inotify fd");
    const struct inotify_event &event = *(const struct inotify_event *)ptr;
    const size_t total = sizeof event + event.len;
    if((size_t)n < total)
      throw SystemError("short read from inotify fd");
    
    // See if the file contents changed.
    if(event.wd == file_wd) {
      // Read as much as we can from the file, whatever happened
      readLines();
      // If the file was deleted or renamed away, we're done with this version
      // of it.  Note that we don't seem to get (at least) IN_DELETE_SELF
      // reliably!  However we handle the cases that ought to produce it below
      // in the dir_wd checking too.
      if(event.mask & (IN_MOVE_SELF|IN_DELETE_SELF)) {
        // Close the file.
        closeFile();
      }
    }

    if(event.wd == dir_wd
       && event.len
       && !strcmp(event.name, base.c_str())) {
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

void Watcher::readLines() {
  int c;
  for(;;) {
    // Read the line up to the next newline or EOF
    while((c = getc(fp)) != EOF) {
      line += c;
      if(c == '\n')
        break;
    }
    if(c == '\n') {
      // We got a complete line
      processLine(line);
      line.clear();
    } else {
      // We did not get a complete line.  If it was an error, close it
      // and throw.
      if(ferror(fp)) {
        closeFile();
        throw IOError("reading " + path, errno);
      }
      // Otherwise it must be EOF.
      // Make sure future reads, after the file is extended, will succeed.
      clearerr(fp);
      // Done for now.  If the file is deleted before a newline is read,
      // closeFile() will handle the partial line.
      return;
    }
  }
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
