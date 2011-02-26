#include <config.h>
#include "Watcher.h"
#include "ConfFile.h"
#include "Address.h"
#include "Regex.h"
#include <sys/select.h>
#include <cerrno>
#include <cstdlib>

bool debug;
ConfFile *config;

class BanWatcher: public Watcher {
public:
  BanWatcher(const std::string &path): Watcher(path) {}

  void processLine(const std::string &line) {
    std::vector<regmatch_t> matches;
    for(size_t i = 0; i < config->patterns.size(); ++i) {
      const ConfFile::Match &m = config->patterns[i];
      if(m.regex.execute(line, matches) == 0) {
        // We got one
        std::string address(line, 
                            matches[m.capture].rm_so,
                            matches[m.capture].rm_eo - matches[m.capture].rm_so);
        fprintf(stderr, "matched [%s]\n", address.c_str());
      }
    }
  }
};

int main(int argc, char **argv) {
  int n;
  bool background = false;              // TODO should be true
  const char *conffile = "/etc/banjammer.conf";

  while((n = getopt(argc, argv, "dfc:")) >= 0) {
    switch(n) {
    case 'd':
      debug = true;
      break;
    case 'f':
      background = false;
      break;
    case 'c':
      conffile = optarg;
      break;
    default:
      exit(1);
    }
  }
  try {
    // Read configuration
    config = new ConfFile(conffile);
    // Become a daemon
    if(background)
      daemon(0, 1);
    // TODO catch SIGHUP
    for(;;) {
      std::vector<BanWatcher *> watchers;
      for(size_t i = 0; i < config->files.size(); ++i)
        watchers.push_back(new BanWatcher(config->files[i]));
      for(;;) {
        int maxfd = -1;
        fd_set fds;
        FD_ZERO(&fds);
        for(size_t i = 0; i < watchers.size(); ++i) {
          const int fd = watchers[i]->pollfd();
          FD_SET(fd, &fds);
          if(fd > maxfd)
            maxfd = fd;
        }
        n = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if(n < 0) {
          if(errno == EINTR)
            continue;
          fprintf(stderr, "select: %s\n", strerror(errno)); // TODO
          exit(-1);
        }
        for(size_t i = 0; i < watchers.size(); ++i) {
          const int fd = watchers[i]->pollfd();
          if(FD_ISSET(fd, &fds))
            watchers[i]->work();
        }
        // TODO exit loop on SIGHUP
      }
      for(size_t i = 0; i < watchers.size(); ++i)
        delete watchers[i];
    }
  } catch(std::runtime_error &e) {
    fprintf(stderr, "ERROR: %s\n", e.what()); // TODO
    exit(1);
  }
  exit(0);
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
