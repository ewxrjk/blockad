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
#include "Regex.h"

// Regex ----------------------------------------------------------------------

Regex::Regex(): creg(NULL) {
}

Regex::Regex(const std::string &regex, int cflags, bool basic): creg(NULL) {
  compile(regex, cflags, basic);
}

void Regex::compile(const std::string &regex, int cflags, bool basic) {
  if(creg && --creg->refcount)
    delete creg;
  if(!basic)
    cflags |= REG_EXTENDED;
  creg = new CompiledRegex(cflags);
  int rc = regcomp(&creg->reg, regex.c_str(), cflags);
  if(rc)
    throw CompilationError(rc, &creg->reg);
  creg->compiled = true;
}

Regex::Regex(const Regex &that): creg(that.creg) {
  if(creg)
    ++creg->refcount;
}

Regex &Regex::operator=(const Regex &that) {
  if(this != &that) {
    if(creg && --creg->refcount == 0)
      delete creg;
    creg = that.creg;
    if(creg)
      ++creg->refcount;
  }
  return *this;
}

Regex::~Regex() {
  if(creg && --creg->refcount == 0)
    delete creg;
}

int Regex::execute(const std::string &s,
                   std::vector<regmatch_t> &matches,
                   int eflags,
                   size_t nmatch) const {
  matches.resize(nmatch);
  int rc = regexec(&creg->reg, s.c_str(), nmatch, &matches[0], eflags);
  // Strip any unused matches table entries
  if(rc == 0) {
    while(matches.size() > 0 && matches.back().rm_so == -1)
      matches.pop_back();
  }
  return rc;
}

int Regex::execute(const std::string &s,
                   int eflags) const {
  if(creg->cflags & REG_NOSUB)
    return regexec(&creg->reg, s.c_str(), 0, NULL, eflags);
  else {
    std::vector<regmatch_t> matches;
    return execute(s, matches, eflags, default_nmatches);
  }
}

// Regex::Error ---------------------------------------------------------------

std::string Regex::Error::format(int rc, const regex_t *preg) {
  size_t size = regerror(rc, preg, NULL, 0);
  char *buffer = new char[size];
  regerror(rc, preg, buffer, size);
  std::string r(buffer);
  delete[] buffer;
  return r;
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
