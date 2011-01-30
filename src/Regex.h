#ifndef REGEX_H
#define REGEX_H

#include <sys/types.h>
#include <regex.h>
#include <string>
#include <vector>
#include <stdexcept>

class Regex {
public:
  static const int default_nmatches = 64;

  // Compile REGEX.  Throw CompilationError on failure.
  //
  // By default REG_EXTENDED is always used.  You can suppress this by passing
  // 'basic' as true.  The justification is that BREs are obsolete.
  Regex(const std::string &regex, int cflags = 0, bool basic = false);

  ~Regex() {
    regfree(&reg);
  }

  // Execute the regex.  Returns 0 for a match and REG_NOMATCH for a match
  // failure.  matches is resized to be nmatch for the call, and then resized
  // to strip any trailing unused entries on success.
  int execute(const std::string &s,
              std::vector<regmatch_t> &matches,
              int eflags = 0,
              size_t nmatch = default_nmatches) const;

  // Execute the regex but do not capture.
  int execute(const std::string &s,
              int eflags = 0) const;

  // Simple match test
  inline bool matches(const std::string &s, int eflags = 0) const {
    return execute(s, eflags) == 0;
  }

  // Base class for Regex errors
  class Error: public std::runtime_error {
  public:
    Error(const std::string &s): std::runtime_error(s) {}
    Error(int rc, const regex_t *preg): std::runtime_error(format(rc, preg)) {}
  private:
    static std::string format(int rc, const regex_t *preg);
  };

  // Compilation failed
  class CompilationError: public Error {
  public:
    CompilationError(int rc, const regex_t *preg): Error(rc, preg) {}
  };

private:
  regex_t reg;
  int cflags;
};

#endif /* REGEX_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
