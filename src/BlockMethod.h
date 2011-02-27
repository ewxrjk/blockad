#ifndef BLOCKMETHOD_H
#define BLOCKMETHOD_H

#include <string>
#include <map>

class Address;

class BlockMethod {
public:
  BlockMethod(const char *name);
  virtual ~BlockMethod();

  static const BlockMethod *find(const std::string &name);

  virtual bool block(const Address &a) const = 0;

private:
  static std::map<std::string,const BlockMethod *> *registry;
};

#endif /* BLOCKMETHOD_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
