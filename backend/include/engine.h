#ifndef PROTECTOR_ENGINE_H
#define PROTECTOR_ENGINE_H
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "database.h"
#include "lexer.h"
class BookStoreEngineClass {
  std::string config_dir;
  UserDataBase user_data_base;
  BookDataBase book_data_base;
  LogDataBase log_data_base;
  std::unordered_map<std::string, int> login_count;
  bool is_server;

 public:
  BookStoreEngineClass() = delete;
  BookStoreEngineClass(std::string __config_dir, bool __is_server);
  std::string QueryUserInfo(const std::string &user_name);
  std::vector<std::string> Execute(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteSu(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteLogout(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteRegister(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecutePasswd(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteUserAdd(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteDelete(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteShow(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteBuy(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteSelect(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteMOdify(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteImport(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteShowFinance(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteLog(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
  std::vector<std::string> ExecuteReport(
      const std::string &cmd,
      std::stack<std::pair<std::string, int>> &login_stack);
};
#endif  // PROTECTOR_ENGINE_H