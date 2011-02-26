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

#if __linux__
#include "Ban.h"
#include "log.h"
#include <cstdlib>

bool BanLinux(const Address &a) {
  std::string command;
  if(a.is4())
    command = "iptables -I INPUT -j REJECT -s " + a.as4();
  else
    command = "ip6tables -I INPUT -j REJECT -s " + a.as6();
  debug("command: %s", command.c_str());
  int rc = system(command.c_str());
  if(rc) {
    error("ban command exited %#x: %s", rc, command.c_str());
    return false;
    // TODO we should capture iptables' stderr and report that
  }
  return true;
}

#endif
