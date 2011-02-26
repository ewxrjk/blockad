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
