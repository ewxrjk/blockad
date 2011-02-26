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
#include "StdioFile.h"
#include "log.h"
#include <sstream>
#include <cerrno>
#include <cstdlib>
#include <climits>

// ConfFile -------------------------------------------------------------------

ConfFile::ConfFile(): rate_max(rate_max_default),
                      rate_interval(rate_interval_default) {
}

ConfFile::ConfFile(const std::string &path_): 
  rate_max(rate_max_default),
  rate_interval(rate_interval_default),
  path(path_) {
  parse();
}

void ConfFile::parse() {
  StdioFile f(path, "r");
  std::string line;
  lineno = 0;
  while(f.readline(line)) {
    ++lineno;
    try {
      parseLine(line);
    } catch(SyntaxError &e) {
      // SyntaxError already has location information
      throw e;
    } catch(std::runtime_error &e) {
      // Attach location information to any other errors
      throw SyntaxError(this, e);
    }
  }
}

void ConfFile::parseLine(const std::string &line) {
  std::vector<std::string> bits;

  if(!splitLine(line, bits))
    return;                             // skip blank lines
  if(bits[0] == "watch") {
    // watch PATH
      
    // Syntax check
    if(bits.size() < 2)
      throw SyntaxError(this, "missing argument to 'watch'");
    else if(bits.size() > 2)
      throw SyntaxError(this, "excess arguments to 'watch'");

    // Stash the filename
    files.push_back(bits[1]);
  } else if(bits[0] == "address") {
    // address REGEXP [CAPTURE-NUMBER]

    // Syntax check
    if(bits.size() < 2)
      throw SyntaxError(this, "missing argument to 'address'");
    else if(bits.size() > 3)
      throw SyntaxError(this, "excess arguments to 'address'");

    long c;
    Regex r;

    // Pick up the capture count.  It must be a decimal integer.
    if(bits.size() == 3) {
      char *end;
      errno = 0;
      c = strtol(bits[2].c_str(), &end, 10);
      if(errno)
        throw SyntaxError(this, std::string("invalid capture argument to 'address': ") + strerror(errno));
      if(end == bits[2].c_str() || *end || c < 1 || c > INT_MAX)
        throw SyntaxError(this, "invalid capture argument to 'address'");
    } else
      c = 1;

    // Compile the regex.  We do it here for the syntax checking.
    r.compile(bits[1]);

    // Reject invalid regex and capture count combinations
    if(r.captures() == 0)
      throw SyntaxError(this, "no captures in regexp");
    if((unsigned long)c > r.captures())
      throw SyntaxError(this, "capture out of range");

    // Stash the pair
    patterns.push_back(Match(r, c));
  } else if(bits[0] == "rate") {
    // rate COUNT/[minute|hour|day|week]
      
    // Syntax check
    if(bits.size() < 2)
      throw SyntaxError(this, "missing argument to 'rate'");
    else if(bits.size() > 2)
      throw SyntaxError(this, "excess arguments to 'rate'");

    // Extract the count
    errno = 0;
    char *end;
    long count = strtol(bits[1].c_str(), &end, 10);
    if(errno)
      throw SyntaxError(this, std::string("invalid count for 'rate': ") + strerror(errno));
    if(end == bits[1].c_str() || count < 1 || (unsigned long)count > UINT_MAX)
      throw SyntaxError(this, "invalid count for 'rate'");
    rate_max = count;

    // Extract the interval
    if(!strcmp(end, "/minute")
       || !strcmp(end, "/min"))
      rate_interval = 60;
    else if(!strcmp(end, "/hour"))
      rate_interval = 60 * 60;
    else if(!strcmp(end, "/day"))
      rate_interval = 60 * 60 * 24;
    else if(!strcmp(end, "/week"))
      rate_interval = 60 * 60 * 24 * 7;
    else
      throw SyntaxError(this, "invalid interval for 'rate'");
  } else if(bits[0] == "exempt") {
    // exempty ADDRESS[/MASK]

    // Syntax check
    if(bits.size() < 2)
      throw SyntaxError(this, "missing argument to 'exempt'");
    else if(bits.size() > 2)
      throw SyntaxError(this, "excess arguments to 'exempt'");
    AddressPattern p(bits[1]);
    exempted.push_back(p);
  } else
    throw SyntaxError(this, "unrecognized command '" + bits[0] + "'");
}

size_t ConfFile::splitLine(const std::string &line,
                           std::vector<std::string> &bits) {
  bits.clear();
  size_t pos = 0;
  std::string bit;
  while(pos < line.size()) {
    char ch = line.at(pos++);
    switch(ch) {
    case '#':                   // comment marker
      pos = line.size();
      break;
    case ' ':                   // skip blanks
      break;
    case '"':                   // two kinds of quoted string
    case '\'':
      pos = parseString(line, pos, bit, ch);
      bits.push_back(bit);
      break;
    default:
      bit.clear();
      bit += ch;
      while(pos < line.size() && line.at(pos) != ' ')
        bit += line.at(pos++);
      bits.push_back(bit);
      break;
    }
  }
  return bits.size();
}

size_t ConfFile::parseString(const std::string &line,
                             size_t pos,
                             std::string &bit,
                             char q) {
  bit.clear();
  while(pos < line.size()) {
    char ch = line.at(pos++);
    // Check for close quote
    if(ch == q)
      return pos;               // done
    if(ch != '\\' || q == '\'') {
      // Unescaped character; for '' strings, all characters count as
      // unescaped.
      bit += ch;
    } else {
      // C-style escapes
      if(pos > line.size())
        throw SyntaxError(this, "unterminated escape sequence");
      ch = line.at(pos++);
      switch(ch) {
      case 'a': bit += '\a'; break;
      case 'b': bit += '\b'; break;
      case 't': bit += '\t'; break;
      case 'n': bit += '\n'; break;
      case 'v': bit += '\v'; break;
      case 'f': bit += '\f'; break;
      case 'r': bit += '\r'; break;
      case '\'': case '"': case '\\': case '?': bit += ch; break;
      case 'x': {
        int value = 0;
        while(pos < line.size()) {
          ch = line.at(pos);
          if(!isxdigit(ch))
            break;
          ++pos;
          value = 16 * value + decodeHex(ch);
          if(value > 255)
            throw SyntaxError(this, "hexadeciaml escape sequence out of range");
        }
        bit += value;
        break;
      }
      case '0': case '1': case '2': case '3':
      case '4': case '5': case '6': case '7': {
          int value = ch - '0';
          if(pos < line.size() && line.at(pos) >= '0' && line.at(pos) <= '7') {
            value = value * 8 + line.at(pos++) - '0';
            if(pos < line.size() && line.at(pos) >= '0' && line.at(pos) <= '7')
              value = value * 8 + line.at(pos++) - '0';
          }
          if(value > 255)
            throw SyntaxError(this, "octal escape sequence out of range");
          bit += value;
          break;
        }
        // TODO \uNNNN and \UNNNNNNNN would be nice
      default:
        throw SyntaxError(this, "unrecognized escape sequence");
      }
    }
  }
  // If we exit the loop we must have hit the end of the line without
  // seeing a close quote
  throw SyntaxError(this, "unterminated string literal");
}

int ConfFile::decodeHex(char ch) {
  if(ch >= '0' && ch <= '9') return ch - '0';
  else if(ch >= 'a' && ch <= 'f') return ch - ('a' - 10);
  else return ch - ('A' - 10);
}

// ConfFile::SyntaxError ------------------------------------------------------

std::string ConfFile::SyntaxError::format(const ConfFile *cf,
                                          const std::string &s) {
  std::ostringstream ss;

  ss << cf->path << ':' << cf->lineno << ": " << s;
  return ss.str();
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
