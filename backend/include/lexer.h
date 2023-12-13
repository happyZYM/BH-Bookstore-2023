#ifndef PROTECTOR_LEXER_H
#define PROTECTOR_LEXER_H
#include <regex>
#include <sstream>
#include <string>
#include <vector>
bool CommandShowLexer(const std::string &command, std::string &ISBN,
                      std::string &name, std::string &author,
                      std::string &keyword);
#endif  // PROTECTOR_LEXER_H