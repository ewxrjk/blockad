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
#include "Address.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>

// AddressBase ----------------------------------------------------------------

// Convert s[pos,pos+n) to addr.  Return 4 or 6 depending on the syntax used
// (the result is always a possibly v4-mapped IPv6 address.  Returns -1 for an
// invalid address (the exception is raised in the derived class).
int AddressBase::convert(struct in6_addr &addr,
                         const std::string &ss, 
                         std::string::size_type pos,
                         std::string::size_type n) {
  if(n > ss.size() - pos)
    n = ss.size() - pos;
  std::string s(ss, pos, n);
  if(s.find(':') != std::string::npos) {
    // Looks like an IPv6 address
    if(!inet_pton(AF_INET6, s.c_str(), addr.s6_addr))
      throw InvalidAddress();
    return 6;
  } else {
    // Looks like an IPv4 address.  Store it as a v4-mapped IPv6 address.
    if(!inet_pton(AF_INET, s.c_str(), &addr.s6_addr[12]))
      throw InvalidAddress();
    memset(&addr, 0, 10);
    addr.s6_addr[10] = addr.s6_addr[11] = 0xFF;
    return 4;
  }
}

// Address --------------------------------------------------------------------

void Address::assign(const std::string &s) {
  if(convert(address, s) < 0)
    throw InvalidAddress();
}

bool Address::is4() const {
  for(size_t n = 0; n < 10; ++n)
    if(address.s6_addr[n])
      return false;
  return address.s6_addr[10] == 0xFF && address.s6_addr[11] == 0xFF;
}

std::string Address::as4() const {
  char buffer[INET_ADDRSTRLEN];
  if(!is4())
    throw InvalidAddress();
  return inet_ntop(AF_INET, address.s6_addr + 12, buffer, sizeof buffer);
}

std::string Address::as6() const {
  char buffer[INET6_ADDRSTRLEN];
  return inet_ntop(AF_INET6, 
                   address.s6_addr, buffer, sizeof buffer);
}

std::string Address::asString() const {
  return is4() ? as4() : as6();
}

// AddressPattern -------------------------------------------------------------

void AddressPattern::assign(const std::string &s) {
  long maskBits;
  // See if there's a netmask size
  size_t slash = s.find('/');
  // Convert everything up to that point
  int v = convert(network, s, 0, slash);
  if(v < 0)
    throw InvalidAddressPattern();
  if(slash == std::string::npos) {
    // If there was no netmask, match the address exactly
    maskBits = 128;
  } else {
    // Extract the netmask
    errno = 0;
    const char *maskString = s.c_str() + slash + 1;
    char *end;
    maskBits = strtol(maskString, &end, 10);
    if(errno)
      throw InvalidAddressPattern(strerror(errno));
    if(end == maskString || *end || maskBits < 0)
      throw InvalidAddressPattern();
    // Check that the netmask size is in range, and if the address was in V4
    // format then adjust for a V4-mapped V6 address.
    if(v == 4) {
      if(maskBits > 32)
        throw InvalidAddressPattern();
      maskBits += 96;
    } else {
      if(maskBits > 128)
        throw InvalidAddressPattern();
    }
  }
  // Construct the mask itself
  for(long n = 0; n < 16; ++n) {
    if(n < maskBits / 8)
      mask.s6_addr[n] = 0xFF;
    else if(n == maskBits / 8)
      mask.s6_addr[n] = 0xFF << (8 - (maskBits % 8));
    else
      mask.s6_addr[n] = 0;
  }
}

bool AddressPattern::matches(const Address &a) const {
  for(size_t n = 0; n < 16; ++n)
    if((a.address.s6_addr[n] & mask.s6_addr[n]) != network.s6_addr[n])
      return false;
  return true;
}

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
