#include "engine.h"

#include <cctype>
#include <stack>
#include <string>
#include <unordered_map>

#include "bs-utility.h"

BookStoreEngineClass::BookStoreEngineClass(std::string __config_dir,
                                           bool __is_server) {
  config_dir = __config_dir;
  user_data_base.Open(config_dir + "user");
  book_data_base.Open(config_dir + "book");
  log_data_base.Open(config_dir + "log");
  is_server = __is_server;
}
std::vector<std::string> BookStoreEngineClass::Execute(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  for (int i = 0; i < cmd.length(); i++)
    if (std::isspace(cmd[i]) && cmd[i] != ' ')
      return std::vector<std::string>({"Invalid"});
  static std::unordered_map<std::string, OperationType> operation_map = {
      {"su", OperationType::__Ksu},
      {"logout", OperationType::__Klogout},
      {"useradd", OperationType::__Kuseradd},
      {"register", OperationType::__Kregister},
      {"delete", OperationType::__Kdelete},
      {"passwd", OperationType::__Kpasswd},
      {"select", OperationType::__Kselect},
      {"modify", OperationType::__Kmodify},
      {"import", OperationType::__Kimport},
      {"show", OperationType::__Kshow},
      {"buy", OperationType::__Kbuy},
      {"report", OperationType::__Kreport},
      {"log", OperationType::__Klog}};
  std::stringstream ss(cmd);
  std::string head = "";
  ss >> head;
  if (head == "quit" || head == "exit") {
    if (!is_server) BookStore_ZYM::shut_down = true;
    return std::vector<std::string>();
  }
  if (operation_map.find(head) == operation_map.end()) {
    for (int i = 0; i < cmd.length(); i++)
      if (cmd[i] != ' ') return std::vector<std::string>({"Invalid"});
    return std::vector<std::string>();
  }
  switch (operation_map[head]) {
    case OperationType::__Ksu: {
      return ExecuteSu(cmd, login_stack);
    }
    case OperationType::__Klogout: {
      return ExecuteLogout(cmd, login_stack);
    }
    case OperationType::__Kuseradd: {
      return ExecuteUserAdd(cmd, login_stack);
    }
    case OperationType::__Kregister: {
      return ExecuteRegister(cmd, login_stack);
    }
    case OperationType::__Kdelete: {
      return ExecuteDelete(cmd, login_stack);
    }
    case OperationType::__Kpasswd: {
      return ExecutePasswd(cmd, login_stack);
    }
    case OperationType::__Kselect: {
      return ExecuteSelect(cmd, login_stack);
    }
    case OperationType::__Kmodify: {
      return ExecuteMOdify(cmd, login_stack);
    }
    case OperationType::__Kimport: {
      return ExecuteImport(cmd, login_stack);
    }
    case OperationType::__Kshow: {
      ss >> head;
      if (head == "finance") goto dst_showfinance;
      return ExecuteShow(cmd, login_stack);
    }
    case OperationType::__Kshowfinance: {
    dst_showfinance:;
      return ExecuteShowFinance(cmd, login_stack);
    }
    case OperationType::__Kbuy: {
      return ExecuteBuy(cmd, login_stack);
    }
    case OperationType::__Kreport: {
      // throw FatalError("report Not implemented", 2);
      return ExecuteReport(cmd, login_stack);
    }
    case OperationType::__Klog: {
      // throw FatalError("log Not implemented", 3);
      return ExecuteLog(cmd, login_stack);
    }
  }
  throw FatalError("Unknown Command", 5);
  return std::vector<std::string>({cmd});
}

std::vector<std::string> BookStoreEngineClass::ExecuteSu(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteLogout(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteRegister(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecutePasswd(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteUserAdd(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteDelete(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShow(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteBuy(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteSelect(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteMOdify(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteImport(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShowFinance(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteLog(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteReport(
    const std::string &cmd, std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}