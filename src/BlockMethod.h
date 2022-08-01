#ifndef BLOCKMETHOD_H
#define BLOCKMETHOD_H

#include <string>
#include <map>
#include <vector>

class Address;
class ConfFile;
class BlockMethod;

// Base class for types methods of blocking hosts
//
// Each block method should be defined as a subclass, of which exactly one
// (static) instance should be created.  BlockMethod::BlockMethod() will
// register it for retrieval with BlockMethod::find();
class BlockMethodType {
public:
  // Register block method
  BlockMethodType(const char *name);
  virtual ~BlockMethodType();

  // Find a block method by name
  static const BlockMethodType *find(const std::string &name);

  // Apply configuration parameters and create a block method
  //
  // bits is the _whole_ directive, i.e. including 'block <name>'.
  virtual BlockMethod *create(ConfFile *cf,
                              const std::vector<std::string> &bits) const;

  virtual BlockMethod *create() const = 0;

private:
  static std::map<std::string, const BlockMethodType *> *registry;
};

// Parameterized blockers
class BlockMethod {
public:
  virtual ~BlockMethod();

  // Block an address
  virtual bool block(const Address &a) = 0;
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
