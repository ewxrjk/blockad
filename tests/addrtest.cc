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
#include <cassert>
#include <cstdio>

int main() {
  Address a;
  assert(!a.is4());
  assert(a.as6() == "::");
  assert(a.asString() == "::");

  a = "127.0.0.1";
  assert(a.is4());
  assert(a.as4() == "127.0.0.1");
  assert(a.as6() == "::ffff:127.0.0.1");
  assert(a.asString() == "127.0.0.1");

  a = "::1";
  assert(!a.is4());
  assert(a.as6() == "::1");
  assert(a.asString() == "::1");

  AddressPattern p;
  assert(p.matches("127.0.0.1"));
  assert(p.matches("::1"));

  p = "127.0.0.0/8";
  assert(p.matches("127.0.0.1"));
  assert(p.matches("127.255.255.255"));
  assert(p.matches("::ffff:127.0.0.1"));
  assert(!p.matches("128.0.0.0"));
  assert(!p.matches("126.0.0.0"));
  assert(!p.matches("::1"));
  assert(!p.matches("::"));

  p = "ffff::/16";
  assert(!p.matches("127.0.0.1"));
  assert(!p.matches("0.0.0.0"));
  assert(p.matches("ffff::"));
  assert(p.matches("ffff::1"));
  assert(p.matches("ffff:1::1"));
  assert(!p.matches("::"));

  p = "ffff::/16";
  assert(!p.matches("127.0.0.1"));
  assert(!p.matches("0.0.0.0"));
  assert(p.matches("ffff::"));
  assert(p.matches("ffff::1"));
  assert(p.matches("ffff:1::1"));
  assert(!p.matches("::"));

  p = "2001:470:1f09:11ed::/64";
  assert(!p.matches("127.0.0.1"));
  assert(!p.matches("0.0.0.0"));
  assert(p.matches("2001:470:1f09:11ed::12"));
  assert(p.matches("2001:470:1f09:11ed:219:d1ff:fe04:d176"));
  assert(!p.matches("fe80::219:d1ff:fe04:d176"));
  return 0;  
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
