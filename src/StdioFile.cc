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
#include "StdioFile.h"
#include "IOError.h"
#include <cerrno>

// StdioFile ------------------------------------------------------------------

StdioFile::StdioFile(): fp(NULL) {
}

StdioFile::StdioFile(const std::string &path_, const std::string &mode):
  path(path_), fp(fopen(path.c_str(), mode.c_str())) {
  if(!fp)
    throw IOError("opening", path, errno);
}

StdioFile::~StdioFile() {
  if(fp)
    fclose(fp);
}

void StdioFile::open(const std::string &path_, const std::string &mode) {
  if(fp) {
    fclose(fp);
    path.clear();
  }
  path = path_;
  fp = fopen(path.c_str(), mode.c_str());
  if(!fp)
    throw IOError("opening", path, errno);
}

void StdioFile::close() {
  FILE *ofp = fp;
  fp = NULL;
  if(fclose(ofp) < 0)
    throw IOError("closing", path, errno);
}

int StdioFile::readc() {
  int c = getc(fp);
  if(c == EOF && ferror(fp))
    throw IOError("reading", path, errno);
  return c;
}

bool StdioFile::readline(std::string &line) {
  int c;
  line.clear();
  while((c = readc()) != EOF) {
    if(c == '\n')
      return true;
    line += c;
  }
  return !!line.size();
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
