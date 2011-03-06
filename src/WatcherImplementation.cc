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
#include <cerrno>

WatcherImplementation::~WatcherImplementation() {
  if(fp)
    fclose(fp);
}

std::string WatcherImplementation::getDirName(const std::string &p) {
  std::string::size_type s = p.rfind('/');
  if(s == std::string::npos)
    return ".";
  else
    return std::string(p, 0, s + 1);
}

std::string WatcherImplementation::getBaseName(const std::string &p) {
  std::string::size_type s = p.rfind('/');
  if(s == std::string::npos)
    return p;
  else
    return std::string(p, s + 1);
}

void WatcherImplementation::readLines() {
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
        throw Watcher::IOError("reading " + path, errno);
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
