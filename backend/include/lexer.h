#ifndef PROTECTOR_LEXER_H
#define PROTECTOR_LEXER_H
#include <regex>
#include <sstream>
#include <string>
#include <vector>
bool CommandSuLexer(const std::string &command, std::string &user_id,
                    std::string &password);
bool CommandLogoutLexer(const std::string &command);
bool CommandRegisterLexer(const std::string &command, std::string &user_id,
                          std::string &password, std::string &username);
bool CommandPasswdLexer(const std::string &command, std::string &user_id,
                        std::string &old_password, std::string &new_password);
bool CommandUseraddLexer(const std::string &command, std::string &user_id,
                         std::string &password, int &privilege,
                         std::string &username);
bool CommandDeleteLexer(const std::string &command, std::string &user_id);
bool CommandShowLexer(const std::string &command, std::string &ISBN,
                      std::string &name, std::string &author,
                      std::string &keyword);
bool CommandBuyLexer(const std::string &command, std::string &ISBN,
                     int &quantity);
bool CommandSelectLexer(const std::string &command, std::string &ISBN);
bool CommandModifyLexer(const std::string &command, std::string &ISBN,
                        std::string &name, std::string &author,
                        std::string &keyword, double &price);
bool CommandImportLexer(const std::string &command, int &quantity,
                        double &total_cost);
bool CommandShowfinanceLexer(const std::string &command,int &count);
bool KeyWordSpliter(const std::string &keyword, std::vector<std::string> &words);
#endif  // PROTECTOR_LEXER_H