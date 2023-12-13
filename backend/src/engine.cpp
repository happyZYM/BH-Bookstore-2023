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
  if (user_data_base.GetPrevilege("root") == -1) {
    user_data_base.AddUser("root", "sjtu", "root", 7);
  }
}
std::vector<std::string> BookStoreEngineClass::Execute(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
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
      return ExecuteReport(cmd, login_stack);
    }
    case OperationType::__Klog: {
      return ExecuteLog(cmd, login_stack);
    }
  }
  throw FatalError("Unknown Command", 5);
  return std::vector<std::string>({cmd});
}

std::vector<std::string> BookStoreEngineClass::ExecuteSu(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  std::string user_id, password;
  if (!CommandSuLexer(cmd, user_id, password))
    return std::vector<std::string>({"Invalid"});
  if (login_stack.size() > 0 &&
      user_data_base.GetPrevilege(login_stack.top().first) == 7) {
    if (user_data_base.GetPrevilege(user_id) == -1)
      return std::vector<std::string>({"Invalid"});
    login_stack.push(std::make_pair(user_id, ""));
    return std::vector<std::string>();
  }
  if (user_data_base.PAM(user_id, password)) {
    login_stack.push(std::make_pair(user_id, ""));
    return std::vector<std::string>();
  }
  return std::vector<std::string>({"Invalid"});
}

std::vector<std::string> BookStoreEngineClass::ExecuteLogout(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  if (login_stack.empty()) return std::vector<std::string>({"Invalid"});
  login_stack.pop();
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteRegister(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  std::string user_id, password, user_name;
  if (!CommandRegisterLexer(cmd, user_id, password, user_name))
    return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(cmd) != -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.AddUser(user_id, password, user_name, 1);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecutePasswd(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  std::string user_id, current_password, new_password;
  if (!CommandPasswdLexer(cmd, user_id, current_password, new_password))
    return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(user_id) == -1)
    return std::vector<std::string>({"Invalid"});
  if (login_stack.size() > 0 &&
      user_data_base.GetPrevilege(login_stack.top().first) == 7) {
    user_data_base.ChangePassword(user_id, new_password);
    return std::vector<std::string>();
  }
  if (!user_data_base.PAM(user_id, current_password))
    return std::vector<std::string>({"Invalid"});
  user_data_base.ChangePassword(user_id, new_password);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteUserAdd(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 3)
    return std::vector<std::string>({"Invalid"});
  std::string user_id, password, user_name;
  int privilege;
  if (!CommandUseraddLexer(cmd, user_id, password, privilege, user_name))
    return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(user_id) != -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.AddUser(user_id, password, user_name, privilege);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteDelete(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 7)
    return std::vector<std::string>({"Invalid"});
  std::string user_id;
  if (!CommandDeleteLexer(cmd, user_id))
    return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(user_id) == -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.DeleteUser(user_id);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShow(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteBuy(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteSelect(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteMOdify(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteImport(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShowFinance(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteLog(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteReport(
    const std::string &cmd,
    std::stack<std::pair<std::string, std::string>> &login_stack) {
  return std::vector<std::string>();
}