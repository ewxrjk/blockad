#include <config.h>
#include "Regex.h"

// Regex ----------------------------------------------------------------------

Regex::Regex(const std::string &regex, int cflags_, bool basic):
  cflags(cflags_ | (basic ? 0 : REG_EXTENDED)) {
  int rc = regcomp(&reg, regex.c_str(), cflags);
  if(rc)
    throw CompilationError(rc, &reg);
}

int Regex::execute(const std::string &s,
                   std::vector<regmatch_t> &matches,
                   int eflags,
                   size_t nmatch) const {
  matches.resize(nmatch);
  int rc = regexec(&reg, s.c_str(), nmatch, &matches[0], eflags);
  // Strip any unused matches table entries
  if(rc == 0) {
    while(matches.size() > 0 && matches.back().rm_so == -1)
      matches.pop_back();
  }
  return rc;
}

int Regex::execute(const std::string &s,
                   int eflags) const {
  if(cflags & REG_NOSUB)
    return regexec(&reg, s.c_str(), 0, NULL, eflags);
  else {
    std::vector<regmatch_t> matches;
    return execute(s, matches, eflags, default_nmatches);
  }
}

// Regex::Error ---------------------------------------------------------------

std::string Regex::Error::format(int rc, const regex_t *preg) {
  size_t size = regerror(rc, preg, NULL, 0);
  char *buffer = new char[size];
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
