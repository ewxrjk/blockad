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
#include "Regex_.h"
#include <cassert>

int main() {
  Regex r1("spong");
  assert(r1.matches("spong"));
  assert(r1.matches("--- spong ---"));
  assert(r1.matches("--- sponspong ---"));
  assert(r1.matches("--- spong --- spong"));
  assert(!r1.matches("--- spon ---"));
  
  Regex r2("a+", 0, true);      // basic
  assert(r2.matches("a+"));
  assert(!r2.matches("a"));

  Regex r3("a+");
  assert(r3.matches("aaargh"));
  assert(!r3.matches("blblblb"));
  std::vector<regmatch_t> matches;
  assert(r3.execute("aaaargh", matches) == 0);
  assert(matches.size() == 1);
  assert(matches[0].rm_so == 0);
  assert(matches[0].rm_eo == 4);

  Regex r4("address=([0-9\\.]+) user=([[:alnum:]]+)");
  assert(!r4.matches("address=foo user=bar"));
  assert(r4.execute("blah address=1.2.3.4 user=bob bleh", matches) == 0);
  assert(matches.size() == 3);
  assert(matches[0].rm_so == 5);
  assert(matches[0].rm_eo == 29);
  assert(matches[1].rm_so == 13);
  assert(matches[1].rm_eo == 20);
  assert(matches[2].rm_so == 26);
  assert(matches[2].rm_eo == 29);
  
  return 0;
}
