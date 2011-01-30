#include <config.h>
#include "ConfFile.h"
#include <cassert>

int main(void) {
  ConfFile testconf1("testconf1");
  assert(testconf1.files.size() == 4);
  assert(testconf1.files.find("file1") != testconf1.files.end());
  assert(testconf1.files.find("file 2") != testconf1.files.end());
  assert(testconf1.files.find("file 3") != testconf1.files.end());
  assert(testconf1.files.find("\a\b\t\n\v\f\r\\\"\'\?\001\377\x1\x0001\xff") != testconf1.files.end());

  
}
