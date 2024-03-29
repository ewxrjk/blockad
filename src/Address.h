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
#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <stdexcept>
#include <netinet/in.h>
#include <cstring>

// Thrown for an invald address (either by Address or AddressPattern)
class InvalidAddress: public std::runtime_error {
public:
  InvalidAddress(): std::runtime_error("Invalid address") {}
};

// Thrown for an invalid address pattern
class InvalidAddressPattern: public std::runtime_error {
public:
  InvalidAddressPattern(): std::runtime_error("Invalid address pattern") {}
  InvalidAddressPattern(const std::string &w):
      std::runtime_error("Invalid address pattern: " + w) {}
};

// Common functionality for Address and AddressPattern
class AddressBase {
public:
  static int convert(struct in6_addr &addr, const std::string &s,
                     std::string::size_type pos = 0,
                     std::string::size_type n = std::string::npos);
};

// An address
//
// All addresses are represented internally as IPv6 addresses, with
// IPv4 addresses mapped into ::ffff:0:0/32.
class Address: private AddressBase {
public:
  // Default address is ::
  inline Address() {
    memset(&address, 0, sizeof address);
  }

  // Expects an IPv4 dotted-quad or IPv6 colon-separate address
  inline Address(const std::string &s) {
    assign(s);
  }

  inline Address &operator=(const std::string &s) {
    assign(s);
    return *this;
  }

  inline bool operator<(const Address &that) const {
    return memcmp(&address, &that.address, sizeof(struct in6_addr)) < 0;
  }

  void assign(const std::string &s);

  bool is4() const;             // true if a V4 address
  std::string as4() const;      // as V4, or throws
  std::string as6() const;      // alway as V6
  std::string asString() const; // as V4 if possible, else V6
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

  // Expects an IPv4 dotted-quad or IPv6 colon-separate address, optionally
  // followed by /N to indicate the size of the network part.
  inline AddressPattern(const std::string &s) {
    assign(s);
  }

  inline AddressPattern &operator=(const std::string &s) {
    assign(s);
    return *this;
  }

  void assign(const std::string &s);

  // Return true if address A matches this pattern
  bool matches(const Address &a) const;

  // Expects an IPv4 dotted-quad or IPv6 colon-separate address
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
