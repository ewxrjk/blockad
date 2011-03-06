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
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <csignal>

// Make FD nonblocking
void nonblock(int fd);

// Execute a command and return its wait status.  Logs its output if it failed.
int execute(const std::vector<std::string> &command);
int execute(const char *const *argv);

// Process's original signal mask, so that it can be restored for subprocesses.
extern sigset_t original_sigmask;

#endif /* UTILS_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
