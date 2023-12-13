#include "lexer.h"

// clang-format off
/**
 * @brief Lexer for command "su"
 * 
 * @param command The command to be parsed
 * @param user_id The user_id to be logged in
 * @param password The password to be logged in
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * su [UserID] ([Password])?
*/
// clang-format on
bool CommandSuLexer(const std::string &command, std::string &user_id,
                    std::string &password) {
  static std::basic_regex main_pattern(
      R"(^ *su +(?:[0-9a-zA-Z_]{1,30}) +(?:[0-9a-zA-Z_]{1,30})? *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    user_id = "";
    password = "";
    ss >> user_id;
    ss >> password;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "logout"
 * 
 * @param command The command to be parsed
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * logout
*/
// clang-format on
bool CommandLogoutLexer(const std::string &command) {
  static std::basic_regex main_pattern(R"(^ *logout *$)",
                                       std::regex_constants::optimize);
  return std::regex_match(command, main_pattern);
}

// clang-format off
/**
 * @brief Lexer for command "register"
 * 
 * @param command The command to be parsed
 * @param user_id The user_id to be registered
 * @param password The password to be registered
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * register [UserID] [Password] [Username]
*/
// clang-format on
bool CommandRegisterLexer(const std::string &command, std::string &user_id,
                          std::string &password, std::string &username) {
  static std::basic_regex main_pattern(
      R"(^ *register +(?:[0-9a-zA-Z_]{1,30}) +(?:[0-9a-zA-Z_]{1,30}) +(?:[^\s]{1,30}) *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    user_id = "";
    password = "";
    username = "";
    ss >> user_id;
    ss >> password;
    ss >> username;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "passwd"
 * 
 * @param command The command to be parsed
 * @param user_id The user_id to be changed password
 * @param old_password The old password
 * @param new_password The new password
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * passwd [UserID] ([CurrentPassword])? [NewPassword]
*/
// clang-format on
bool CommandPasswdLexer(const std::string &command, std::string &user_id,
                        std::string &old_password, std::string &new_password) {
  static std::basic_regex main_pattern(
      R"(^ *passwd +(?:[0-9a-zA-Z_]{1,30}) +(?:[0-9a-zA-Z_]{1,30})? +(?:[0-9a-zA-Z_]{1,30}) *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    user_id = "";
    old_password = "";
    new_password = "";
    ss >> user_id;
    ss >> old_password;
    ss >> new_password;
    if (new_password == "") {
      new_password = old_password;
      old_password = "";
    }
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "useradd"
 * 
 * @param command The command to be parsed
 * @param user_id The user_id to be added
 * @param password The password to be added
 * @param privilege The privilege to be added
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * useradd [UserID] [Password] [Privilege] [Username]
*/
// clang-format on
bool CommandUseraddLexer(const std::string &command, std::string &user_id,
                         std::string &password, int &privilege,
                         std::string &username) {
  static std::basic_regex main_pattern(
      R"(^ *useradd +(?:[0-9a-zA-Z_]{1,30}) +(?:[0-9a-zA-Z_]{1,30}) +[0-9] +(?:[^\s]{1,30}) *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    user_id = "";
    password = "";
    privilege = 0;
    username = "";
    ss >> user_id;
    ss >> password;
    ss >> privilege;
    ss >> username;
    if (privilege != 0 && privilege != 1 && privilege != 3 && privilege != 7)
      return false;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "delete"
 * 
 * @param command The command to be parsed
 * @param user_id The user_id to be deleted
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * delete [UserID]
*/
// clang-format on
bool CommandDeleteLexer(const std::string &command, std::string &user_id) {
  static std::basic_regex main_pattern(
      R"(^ *delete +(?:[0-9a-zA-Z_]{1,30}) *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    user_id = "";
    ss >> user_id;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "show"
 * 
 * @param command The command to be parsed
 * @param ISBN The ISBN of the book to be shown
 * @param name The name of the book to be shown
 * @param author The author of the book to be shown
 * @param keyword The keyword of the book to be shown
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?
*/
// clang-format on
bool CommandShowLexer(const std::string &command, std::string &ISBN,
                      std::string &name, std::string &author,
                      std::string &keyword) {
  static std::basic_regex main_pattern(
      R"(^ *show(?: +-ISBN=(?:\S{1,20})| +-name=\"(?:[^\s"]{1,60})\"| +-author=\"(?:[^\s"]{1,60})\"| +-keyword=\"(?:[^\s"]{1,60})\")* *$)",
      std::regex_constants::optimize);

  bool has_ISBN = false;
  bool has_name = false;
  bool has_author = false;
  bool has_keyword = false;
  ISBN = "";
  name = "";
  author = "";
  keyword = "";
  if (std::regex_match(command, main_pattern)) {
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
    for (int i = 0; i < keyword.length(); i++)
      if (keyword[i] == '|') return false;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "buy"
 * 
 * @param command The command to be parsed
 * @param ISBN The ISBN of the book to be bought
 * @param quantity The quantity of the book to be bought
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * buy [ISBN] [Quantity]
*/
// clang-format on
bool CommandBuyLexer(const std::string &command, std::string &ISBN,
                     int &quantity) {
  static std::basic_regex main_pattern(
      R"(^ *buy +(?:\S{1,20}) +[0-9]{1,10} *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    ISBN = "";
    quantity = 0;
    ss >> ISBN;
    long long quantity_tmp = 0;
    ss >> quantity_tmp;
    if (quantity_tmp > 2147483647) return false;
    quantity = quantity_tmp;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "select"
 * 
 * @param command The command to be parsed
 * @param ISBN The ISBN of the book to be selected
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * select [ISBN]
*/
// clang-format on
bool CommandSelectLexer(const std::string &command, std::string &ISBN) {
  static std::basic_regex main_pattern(R"(^ *select +(?:\S{1,20}) *$)",
                                       std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    ISBN = "";
    ss >> ISBN;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "modify"
 * 
 * @param command The command to be parsed
 * @param ISBN The ISBN of the book to be modified
 * @param name The name of the book to be modified
 * @param author The author of the book to be modified
 * @param keyword The keyword of the book to be modified
 * @param price The price of the book to be modified
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" | -price=[Price])+
*/
// clang-format on
bool CommandModifyLexer(const std::string &command, std::string &ISBN,
                        std::string &name, std::string &author,
                        std::string &keyword, double &price) {
  static std::basic_regex main_pattern(
      R"(^ *modify(?: +-ISBN=(?:\S{1,20})| +-name=\"(?:[^\s"]{1,60})\"| +-author=\"(?:[^\s"]{1,60})\"| +-keyword=\"(?:[^\s"]{1,60})\"| +-price=[0-9]{1,10}\.[0-9]{2})+ *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    ISBN = "";
    name = "";
    author = "";
    keyword = "";
    price = 0;
    while (ss >> token) {
      if (token[1] == 'I') {
        ISBN = token.substr(6);
      } else if (token[1] == 'n') {
        name = token.substr(6 + 1, token.size() - 7 - 1);
      } else if (token[1] == 'a') {
        author = token.substr(8 + 1, token.size() - 9 - 1);
      } else if (token[1] == 'k') {
        keyword = token.substr(9 + 1, token.size() - 10 - 1);
      } else if (token[1] == 'p') {
        price = std::stod(token.substr(7));
      } else
        return false;
    }
    for (int i = 0; i < keyword.length(); i++)
      if (keyword[i] == '|') return false;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "import"
 * 
 * @param command The command to be parsed
 * @param quantity The quantity of the book to be imported
 * @param total_cost The total cost of the book to be imported
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * import [Quantity] [TotalCost]
*/
// clang-format on
bool CommandImportLexer(const std::string &command, int &quantity,
                        double &total_cost) {
  static std::basic_regex main_pattern(
      R"(^ *import +[0-9]{1,10} +[0-9]{1,10}\.[0-9]{2} *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    quantity = 0;
    total_cost = 0;
    long long quantity_tmp = 0;
    ss >> quantity_tmp;
    if (quantity_tmp > 2147483647) return false;
    quantity = quantity_tmp;
    ss >> total_cost;
    return true;
  } else
    return false;
}

// clang-format off
/**
 * @brief Lexer for command "showfinance"
 * 
 * @param command The command to be parsed
 * @param count The count of the finance to be shown
 * 
 * @return true if the command is valid
 * 
 * @note The command is valid if and only if it follows the following grammar:
 * show finance ([Count])?
*/
// clang-format on
bool CommandShowfinanceLexer(const std::string &command, int &count) {
  static std::basic_regex main_pattern(
      R"(^ *show +finance(?: +[0-9]{1,10})? *$)",
      std::regex_constants::optimize);
  if (std::regex_match(command, main_pattern)) {
    std::stringstream ss(command);
    std::string token;
    ss >> token;
    long long count_tmp = -1;
    ss >> count_tmp;
    if (count_tmp > 2147483647) return false;
    count = count_tmp;
    return true;
  } else
    return false;
}