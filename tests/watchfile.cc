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
#include <cstdio>

class MyWatcher: public Watcher {
public:
  MyWatcher(const std::string &path): Watcher(path) {}
  
  void processLine(const std::string &line) {
    printf("%s", line.c_str());
    if(line.find('\n') == std::string::npos)
      printf(" [no trailing newline]\n");
    fflush(stdout);
    if(ferror(stdout))
      throw std::runtime_error(std::string("error writing to stdout ") + strerror(errno));
  }
};

int main(int argc, char **argv) {
  if(argc != 2)
    fprintf(stderr, "Usage: watchfile PATH\n");
  MyWatcher w(argv[1]);
  for(;;)
    w.work();
}
