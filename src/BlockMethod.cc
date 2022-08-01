#include <config.h>
#include "BlockMethod.h"
#include "ConfFile.h"

BlockMethodType::BlockMethodType(const char *name) {
  if(!registry)
    registry = new std::map<std::string, const BlockMethodType *>();
  (*registry)[name] = this;
}

BlockMethodType::~BlockMethodType() {}

const BlockMethodType *BlockMethodType::find(const std::string &name) {
  if(registry) {
    std::map<std::string, const BlockMethodType *>::const_iterator it;
    it = registry->find(name);
    if(it != registry->end())
      return it->second;
  }
  return NULL;
}

BlockMethod *
BlockMethodType::create(ConfFile *cf,
                        const std::vector<std::string> &bits) const {
  if(bits.size() > 2)
    throw ConfFile::SyntaxError(cf, "excess arguments to 'block'");
  return create();
}

std::map<std::string, const BlockMethodType *> *BlockMethodType::registry;

BlockMethod::~BlockMethod() {}

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
