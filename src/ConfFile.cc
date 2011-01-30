#include <config.h>
#include "ConfFile.h"
#include "StdioFile.h"
#include <sstream>

// ConfFile -------------------------------------------------------------------

ConfFile::ConfFile(const std::string &path_): path(path_) {
  parse();
}

void ConfFile::reload() {
  ConfFile newConf(path);       // throws on error
  *this = newConf;
}

void ConfFile::parse() {
  StdioFile f(path, "r");
  std::string line;
  std::vector<std::string> bits;
  lineno = 0;
  while(f.readline(line)) {
    ++lineno;
    if(!splitLine(line, bits))
      continue;                 // skip blank lines
    if(bits[0] == "watch") {
      if(bits.size() < 2)
        throw SyntaxError(this, "missing argument to 'watch'");
      else if(bits.size() > 2)
        throw SyntaxError(this, "excess arguments to 'watch'");
      files.insert(bits[1]);
    } else
      throw SyntaxError(this, "unrecognized directive '" + bits[0] + "'");
  }
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
