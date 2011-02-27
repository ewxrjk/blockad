#include <config.h>
#include "BlockMethod.h"
#include "ConfFile.h"

BlockMethod::BlockMethod(const char *name) {
  if(!registry)
    registry = new std::map<std::string,BlockMethod *>();
  (*registry)[name] = this;
}

BlockMethod::~BlockMethod() {
}

BlockMethod *BlockMethod::find(const std::string &name) {
  if(registry) {
    std::map<std::string,BlockMethod *>::const_iterator it;
    it = registry->find(name);
    if(it != registry->end())
      return it->second;
  }
  return NULL;
}

void BlockMethod::parameterize(ConfFile *cf,
                               const std::vector<std::string> &bits) {
  if(bits.size() > 2)
    throw ConfFile::SyntaxError(cf, "excess arguments to 'block'");
}

std::map<std::string,BlockMethod *> *BlockMethod::registry;

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
