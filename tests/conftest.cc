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
