#ifndef BLOCKMETHOD_H
#define BLOCKMETHOD_H

#include <string>
#include <map>
#include <vector>

class Address;
class ConfFile;

// Base class for methods of blocking hosts
//
// Each block method should be defined as a subclass, of which exactly one
// (static) instance should be created.  BlockMethod::BlockMethod() will
// register it for retrieval with BlockMethod::find();
class BlockMethod {
public:
  // Register block method
  BlockMethod(const char *name);
  virtual ~BlockMethod();

  // Find a block method by name
  static BlockMethod *find(const std::string &name);

  // Apply configuration parameters
  //
  // bits is the _whole_ directive, i.e. including 'block <name>'.
  //
  // The default implementation reports an error if any extra parameters are
  // specified.
  virtual void parameterize(ConfFile *cf,
                            const std::vector<std::string> &bits);

  // Block an address
  virtual bool block(const Address &a) = 0;

private:
  static std::map<std::string,BlockMethod *> *registry;
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
