#include "lexer.h"
bool CommandShowLexer(const std::string &command, std::string &ISBN,
                      std::string &name, std::string &author,
                      std::string &keyword) {
  static std::basic_regex main_pattern(
      R"(^ *show(?: +-ISBN=(?:\S{1,20})| +-name=\"(?:[^\s"]{1,60})\"| +-author=\"(?:[^\s"]{1,60})\"| +-keyword=\"(?:[^\s"]{1,60})\")* *$)",
      std::regex_constants::optimize);

  std::smatch results;
  bool has_ISBN = false;
  bool has_name = false;
  bool has_author = false;
  bool has_keyword = false;
  ISBN = "";
  name = "";
  author = "";
  keyword = "";
  if (std::regex_match(command, results, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    while (ss >> token) {
      if (token[1] == 'I') {
        if (has_ISBN) return false;
        has_ISBN = true;
        ISBN = token.substr(6);
      } else if (token[1] == 'n') {
        if (has_name) return false;
        has_name = true;
        name = token.substr(6 + 1, token.size() - 7 - 1);
      } else if (token[1] == 'a') {
        if (has_author) return false;
        has_author = true;
        author = token.substr(8 + 1, token.size() - 9 - 1);
      } else if (token[1] == 'k') {
        if (has_keyword) return false;
        has_keyword = true;
        keyword = token.substr(9 + 1, token.size() - 10 - 1);
      } else
        return false;
    }
    return true;
  } else
    return false;
}