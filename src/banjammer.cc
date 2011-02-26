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
#include "ConfFile.h"
#include "Address.h"
#include "Regex.h"
#include "log.h"
#include "utils.h"
#include <sys/select.h>
#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <map>
#include <deque>

static int signal_pipe[2];
static ConfFile *config;

struct AddressData {
  std::deque<time_t> times;
  bool banned;

  AddressData(): banned(false) {}
};


class BanWatcher: public Watcher {
public:
  BanWatcher(const std::string &path): Watcher(path) {}

  // Called when a line is read from a logfile
  void processLine(const std::string &line) {
    std::vector<regmatch_t> matches;
    for(size_t i = 0; i < config->patterns.size(); ++i) {
      const ConfFile::Match &m = config->patterns[i];
      if(m.regex.execute(line, matches) == 0) {
        // We got one
        std::string address(line, 
                            matches[m.capture].rm_so,
                            matches[m.capture].rm_eo - matches[m.capture].rm_so);
        detectedAddress(Address(address));
      }
    }
  }

private:
  static std::map<Address,AddressData> addressData;

  // Called when an address is detected
  void detectedAddress(const Address &a) {
    debug("detected %s", a.asString().c_str());
    // Honor exemption list
    for(size_t i = 0; i < config->exempted.size(); ++i)
      if(config->exempted[i].matches(a)) {
        debug("%s is exempted", a.asString().c_str());
        return;
      }
    // Find (or create) the data for this address
    AddressData &ad = addressData[a];
    if(!ad.banned) {
      time_t now;
      time(&now);
      // Strip off too-old detection times
      while(ad.times.size()
            && ad.times.front() < now - config->rate_interval)
        ad.times.pop_front();
      // Add the latest detection time
      ad.times.push_back(now);
      // See if the ban rate has been exceeded
      if(ad.times.size() > config->rate_max) {
        banAddress(a);
        ad.banned = true;
      }
    }
  }

  // Ban an address
  void banAddress(const Address &a) {
    info("banning %s", a.asString().c_str());
    // Synthesize the firewall command.  The details differ depending on
    // address family.  (And for platform...)
    std::string command;
#if __linux__
    if(a.is4())
      command = "iptables -I INPUT -j REJECT -s " + a.as4();
    else
      command = "ip6tables -I INPUT -j REJECT -s " + a.as6();
#else
# error Unsupported operating system
#endif
    debug("command: %s", command.c_str());
    int rc = system(command.c_str());
    if(rc)
      error("ban command exited %#x: %s", rc, command.c_str());
    // TODO we should capture iptables' stderr and report that
  }
};

std::map<Address,AddressData> BanWatcher::addressData;

extern "C" {
  static void sighup_handler(int sig) {
    int save_errno = errno;
    write(signal_pipe[1], &sig, 1);
    errno = save_errno;
  }
};

static void updateWatchers(const ConfFile *oldConfig,
                           const ConfFile *newConfig,
                           std::vector<Watcher *> &oldWatchers) {
  size_t i, j;
  std::vector<Watcher *> newWatchers;
  for(i = 0; i < newConfig->files.size(); ++i) {
    // Try and re-use the existing watcher
    if(oldConfig) {
      for(j = 0; j < oldConfig->files.size(); ++j)
        if(newConfig->files[i] == oldConfig->files[i]
           && oldWatchers[j] != NULL)
          break;
      if(j < oldConfig->files.size()) {
        newWatchers.push_back(oldWatchers[j]);
        oldWatchers[j] = NULL;
        continue;
      }
    }
    // We didn't manage to re-use an existing watcher
    newWatchers.push_back(new BanWatcher(newConfig->files[i]));
    nonblock(newWatchers[i]->pollfd());
  }
  // Delete redundant watchers
  for(j = 0; j < oldWatchers.size(); ++j)
    if(oldWatchers[j])
      delete oldWatchers[j];
  oldWatchers = newWatchers;
}

int main(int argc, char **argv) {
  int n;
  bool background = true;
  const char *conffile = "/etc/banjammer.conf";

  while((n = getopt(argc, argv, "dfc:")) >= 0) {
    switch(n) {
    case 'd':
      debugging = true;
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
    // Become a daemon
    if(background) {
      const char *progname;
      if((progname = strrchr(argv[0], '/')))
        ++progname;
      else
        progname = argv[0];
      if(daemon(1, 0)) {
        error("daemon: %s", strerror(errno));
        exit(-1);
      }
      useSyslog(progname);
      debug("daemonized");
    }
    // Read configuration
    config = new ConfFile(conffile);
    info("started");
    // Create the signal notificaiton pipe
    if(pipe(signal_pipe) < 0) {
      error("pipe: %s", strerror(errno));
      exit(-1);
    }
    nonblock(signal_pipe[0]);
    nonblock(signal_pipe[1]);
    // Block SIGHUP
    sigset_t sighup_mask;
    sigemptyset(&sighup_mask);
    sigaddset(&sighup_mask, SIGHUP);
    if(sigprocmask(SIG_BLOCK, &sighup_mask, NULL) < 0) {
      error("sigprocmask: %s", strerror(errno));
      exit(-1);
    }
    // Install a SIGHUP handler
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sighup_handler;
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
      error("sigaction: %s", strerror(errno));
      exit(-1);
    }
    std::vector<Watcher *> watchers;
    // Create the initial watchers
    updateWatchers(NULL, config, watchers);
    for(;;) {
      for(;;) {
        int maxfd;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(signal_pipe[0], &fds);
        maxfd = signal_pipe[0];
        for(size_t i = 0; i < watchers.size(); ++i) {
          const int fd = watchers[i]->pollfd();
          FD_SET(fd, &fds);
          if(fd > maxfd)
            maxfd = fd;
        }
        if(sigprocmask(SIG_UNBLOCK, &sighup_mask, NULL) < 0) {
          error("sigprocmask: %s", strerror(errno));
          exit(-1);
        }
        n = select(maxfd + 1, &fds, NULL, NULL, NULL);
        if(sigprocmask(SIG_BLOCK, &sighup_mask, NULL) < 0) {
          error("sigprocmask: %s", strerror(errno));
          exit(-1);
        }
        if(n < 0) {
          if(errno == EINTR)
            continue;
          error("select: %s", strerror(errno));
          exit(-1);
        }
        for(size_t i = 0; i < watchers.size(); ++i) {
          const int fd = watchers[i]->pollfd();
          if(FD_ISSET(fd, &fds))
            watchers[i]->work();
        }
        if(FD_ISSET(signal_pipe[0], &fds)) {
          char drain[1024];
          while(read(signal_pipe[0], drain, sizeof drain) > 0)
            ;
          break;
        }
      }
      // Read the new config
      ConfFile *newConfig;
      try {
        newConfig = new ConfFile(conffile);
      } catch(std::runtime_error &e) {
        error("%s", e.what());
        continue;
      }
      // Adjust watchers list
      updateWatchers(config, newConfig, watchers);
      // Use the new config hereafter
      delete config;
      config = newConfig;
    }
  } catch(std::runtime_error &e) {
    error("%s", e.what());
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
