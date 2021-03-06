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
#include "ConfFile.h"
#include "Address.h"
#include "StdioFile.h"
#include "IOError.h"
#include "utils.h"
#include "log.h"

class BlockHostsDeny: public BlockMethod {
public:
  BlockHostsDeny(const std::string &path_): path(path_) {}

  bool block(const Address &a) {
    try {
      StdioFile output(path, "a");
      if(a.is4())
        output.printf("ALL: %s\n", a.as4().c_str());
      else
        output.printf("ALL: [%s]\n", a.as6().c_str());
      output.close();
    } catch(IOError &e) {
      error("%s", e.what());
      return false;
    }
    return true;
  }

private:
  std::string path;
};

class BlockHostsDenyType: public BlockMethodType {
public:
  BlockHostsDenyType(): BlockMethodType("hosts.deny") {}

  BlockMethod *create(ConfFile *cf,
                      const std::vector<std::string> &bits) const {
    if(bits.size() > 3)
      throw ConfFile::SyntaxError(cf, "excess arguments to 'block hosts.deny'");
    if(bits.size() > 2)
      return new BlockHostsDeny(bits[2]);
    else
      return create();
  }

  BlockMethod *create() const {
    return new BlockHostsDeny(path_default);
  }

private:
  static const char path_default[];
};

const char BlockHostsDenyType::path_default[] = "/etc/hosts.deny";

static const BlockHostsDenyType block_hosts_deny;

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
