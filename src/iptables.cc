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
#include "BlockMethod.h"
#include "Address.h"
#include "utils.h"

class BlockIptables: public BlockMethod {
public:
  bool block(const Address &a) {
    std::vector<std::string> command;

    if(a.is4())
      command.push_back("iptables");
    else
      command.push_back("ip6tables");
    command.push_back("-I");
    command.push_back("INPUT");
    command.push_back("-j");
    command.push_back("REJECT");
    command.push_back("-s");
    command.push_back(a.asString());
    if(execute(command))
      return false;
    else
      return true;
  }
};

class BlockIptablesType: public BlockMethodType {
public:
  BlockIptablesType(): BlockMethodType("iptables") {}

  BlockMethod *create() const {
    return new BlockIptables();
  }
};

static const BlockIptablesType block_iptables;

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
