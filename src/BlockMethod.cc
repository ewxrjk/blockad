#include <config.h>
#include "BlockMethod.h"

BlockMethod::BlockMethod(const char *name) {
  if(!registry)
    registry = new std::map<std::string,const BlockMethod *>();
  (*registry)[name] = this;
}

BlockMethod::~BlockMethod() {
}

const BlockMethod *BlockMethod::find(const std::string &name) {
  if(registry) {
    std::map<std::string,const BlockMethod *>::const_iterator it;
    it = registry->find(name);
    if(it != registry->end())
      return it->second;
  }
  return NULL;
}

std::map<std::string,const BlockMethod *> *BlockMethod::registry;

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
