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
#include "ConfFile.h"
#include <cassert>

int main(void) {
  ConfFile testconf1("testconf1");
  assert(testconf1.files.size() == 4);
  assert(testconf1.files[0] == "file1");
  assert(testconf1.files[1] == "file 2");
  assert(testconf1.files[2] == "file 3");
  assert(testconf1.files[3] == "\a\b\t\n\v\f\r\\\"\'\?\001\377\x1\x0001\xff");
  // TODO match[]
  // TODO -ve testing
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
