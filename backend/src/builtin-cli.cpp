#include "builtin-cli.h"

#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "bs-utility.h"
#include "engine.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  BookStoreEngineClass engine(config_dir, is_server);
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    std::stack<std::pair<std::string, int>> login_stack;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
      auto result = std::move(engine.Execute(cmd, login_stack));
      debugPrint(cmd);
      for (auto &line : result) {
        std::cout << line << '\n';
        debugPrint(line);
      }
      // debugPrint();
      if (BookStore_ZYM::shut_down) return;
    }
  } else {
    throw FatalError("Server mode has not been implemented yet", 1);
    std::unordered_map<std::string, SessionClass> session_map;
  }
}