#include "builtin-cli.h"

#include <cassert>
#include <iostream>
#include <stack>
#include <string>

#include "bs-utility.h"
#include "engine.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  BookStoreEngineClass engine(config_dir);
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    std::stack<std::string> login_stack;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
      auto result = std::move(engine.Execute(cmd, login_stack));
      for (auto &line : result) {
        std::cout << line << std::endl;
      }
      if (BookStore_ZYM::shut_down) return;
    }
  } else {
    throw FatalError("Not implemented yet", 1);
  }
}