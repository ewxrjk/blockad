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
