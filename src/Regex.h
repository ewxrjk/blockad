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

  Regex();

  // Compile REGEX.  Throw CompilationError on failure.
  //
  // By default REG_EXTENDED is always used.  You can suppress this by passing
  // 'basic' as true.  The justification is that BREs are obsolete.
  Regex(const std::string &regex, int cflags = 0, bool basic = false);

  // Copy constructor/assignment
  Regex(const Regex &);
  Regex &operator=(const Regex &);

  ~Regex();

  void compile(const std::string &regex, int cflags = 0, bool basic = false);

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

  // Count number of captures
  inline size_t captures() const { return creg->reg.re_nsub; }

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
  // We don't (formally) know if copying regex_t to a new address is safe, and
  // it may also not be efficient, so we have just one copy and maintain a
  // reference count in the constructors, operator= and destructor of Regex.
  struct CompiledRegex {
    CompiledRegex(int cflags): refcount(1), cflags(cflags), compiled(false) {}
    ~CompiledRegex() { if(compiled) regfree(&reg); }
    int refcount;
    regex_t reg;
    int cflags;
    bool compiled;
  };

  CompiledRegex *creg;
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
