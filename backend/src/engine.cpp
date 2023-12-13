#include "engine.h"

#include <stack>
#include <string>
#include <unordered_map>

#include "bs-utility.h"

BookStoreEngineClass::BookStoreEngineClass(std::string __config_dir,
                                           bool __is_server) {
  config_dir = __config_dir;
  user_data_base.Open(config_dir + "user");
  is_server = __is_server;
}
std::vector<std::string> BookStoreEngineClass::Execute(
    const std::string &cmd, std::stack<std::string> &login_stack) {
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
    }
    case OperationType::__Klogout: {
    }
    case OperationType::__Kuseradd: {
    }
    case OperationType::__Kregister: {
    }
    case OperationType::__Kdelete: {
    }
    case OperationType::__Kpasswd: {
    }
    case OperationType::__Kselect: {
    }
    case OperationType::__Kmodify: {
    }
    case OperationType::__Kimport: {
    }
    case OperationType::__Kshow: {
      ss >> head;
      if (head == "finance") goto dst_showfinance;
    }
    case OperationType::__Kshowfinance: {
    dst_showfinance:;
    }
    case OperationType::__Kbuy: {
    }
    case OperationType::__Kreport: {
      throw FatalError("report Not implemented", 2);
    }
    case OperationType::__Klog: {
      throw FatalError("log Not implemented", 3);
    }
  }
  return std::vector<std::string>({cmd});
}