#include "Watcher.h"

class MyWatcher: public Watcher {
public:
  MyWatcher(const std::string &path): Watcher(path) {}
  
  void processLine(const std::string &line) {
    printf(">>> %s", line.c_str());
    if(line.find('\n') == std::string::npos)
      printf(" [no trailing newline]\n");
  }
};

int main(int argc, char **argv) {
  if(argc != 2)
    fprintf(stderr, "Usage: watchfile PATH\n");
  MyWatcher w(argv[1]);
  for(;;)
    w.work();
}
