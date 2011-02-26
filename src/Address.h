#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <cstring>

class InvalidAddress: public std::runtime_error {
public:
  InvalidAddress(): std::runtime_error("Invalid address") {}
};

class InvalidAddressPattern: public std::runtime_error {
public:
  InvalidAddressPattern(): std::runtime_error("Invalid address pattern") {}
  InvalidAddressPattern(const std::string &w):
    std::runtime_error("Invalid address pattern: " + w) {}
};

class AddressBase {
public:
  static int convert(struct in6_addr &addr,
                     const std::string &s, 
                     std::string::size_type pos = 0,
                     std::string::size_type n = std::string::npos);
  
};

// An address
class Address: private AddressBase {
public:
  // Default address is ::
  inline Address() {
    memset(&address, 0, sizeof address);
  }
  
  inline Address(const std::string &s) {
    assign(s);
  }

  inline Address &operator=(const std::string &s) { 
    assign(s);
    return *this;
  }
  
  inline bool operator<(const Address &that) const {
    return memcmp(&address, &that.address, sizeof (struct in6_addr)) < 0;
  }

  void assign(const std::string &s);

  bool is4() const;
  std::string as4() const;              // as V4, or throws
  std::string as6() const;              // alway as V6
  std::string asString() const;         // as V4 if possible, else V6
private:
  struct in6_addr address;

  friend class AddressPattern;
};

// A pattern matching addresses
class AddressPattern: private AddressBase {
public:
  inline AddressPattern() {
    memset(&network, 0, sizeof network);
    memset(&mask, 0, sizeof mask);
  }

  inline AddressPattern(const std::string &s) { 
    assign(s);
  }

  inline AddressPattern &operator=(const std::string &s) {
    assign(s);
    return *this;
  }

  void assign(const std::string &s);

  bool matches(const Address &a) const;

  bool matches(const std::string &s) const {
    return matches(Address(s));
  }

private:
  struct in6_addr network;
  struct in6_addr mask;
};

#endif /* ADDRESS_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
