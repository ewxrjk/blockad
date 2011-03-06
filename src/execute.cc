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
#include "IOError.h"
#include "utils.h"
#include "log.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>

sigset_t original_sigmask;

int execute(const std::vector<std::string> &command) {
  std::vector<const char *> argv;
  for(size_t i = 0; i < command.size(); ++i)
    argv.push_back(command[i].c_str());
  argv.push_back(NULL);
  return execute(&argv[0]);
}

int execute(const char *const *argv) {
  int ws, p[2] = { -1, -1 };
  pid_t pid = -1;
  
  try {
    if(debugging) {
      std::string cmd;
      for(size_t i = 0; argv[i]; ++i) {
        if(i)
          cmd += ' ';
        cmd += argv[i];
      }
      debug("execute: %s", cmd.c_str());
    }
    // Format command line
    // Create a pipe for subprocess output
    if(pipe(p) < 0)
      throw IOError("creating pipe", errno);
    // Create subprocess
    switch((pid = fork())) {
    case 0:
      // Plumb in pipe
      if(dup2(p[1], 1) < 0
         || dup2(p[1], 2) < 0
         || close(p[1]) < 0
         || close(p[0]) < 0)
        _Exit(-1);
      // Restore signal mask
      if(sigprocmask(SIG_SETMASK, &original_sigmask, NULL) < 0) {
        fprintf(stderr, "sigprocmask: %s\n", strerror(errno));
        _Exit(-1);
      }
      // Execute the program
      execvp(argv[0], (char *const *)argv);
      fprintf(stderr, "executing %s: %s\n", argv[0], strerror(errno));
      _Exit(-1);
    case -1:
      throw SystemError("creating subprocess", errno);
    }
    if(close(p[1]) < 0)
      throw IOError("closing read end of pipe", errno);
    p[1] = -1;
  
    // Capture output
    std::string output;
    char buffer[512];
    int n;
    for(;;) {
      n = read(p[0], buffer, sizeof buffer);
      if(n > 0)
        output.append(buffer, n);
      else if(n == 0)
        break;
      else if(errno != EINTR)
        throw IOError("reading from pipe", errno);
    }
    close(p[0]);
    p[0] = -1;
  
    // Wait for subprocess
    pid_t rpid;
    for(;;) {
      rpid = waitpid(pid, &ws, 0);
      if(rpid == pid)
        break;
      else if(rpid < 0) {
        if(errno != EINTR)
          throw SystemError("waiting for subprocess", errno);
      }
    }

    if(ws) {
      // Ensure there is a final newline
      if(output.size() && output.at(output.size() - 1) != '\n')
        output += '\n';
      // Dump the captured subprocess output
      std::string::size_type pos = 0;
      while(pos < output.size()) {
        std::string::size_type nl = output.find('\n', pos);
        info("%s: %.*s", argv[0], (int)(nl - pos), output.data() + pos);
        pos = nl + 1;
      }
      error("%s exited with status %#x", argv[0], ws);
    }
  } catch(...) {
    // Close stray file descriptors
    if(p[0] != -1)
      close(p[0]);
    if(p[1] != -1)
      close(p[1]);
    // Reap if possible, but don't wait indefinitely
    if(pid != -1)
      waitpid(pid, NULL, WNOHANG);
    throw;
  }
  
  return ws;
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
