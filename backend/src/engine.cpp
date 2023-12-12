#include "engine.h"

#include <stack>
#include <string>

#include "bs-utility.h"
std::vector<std::string> BookStoreEngineClass::Execute(
    const std::string &cmd, std::stack<std::string> &login_stack) {
  if (cmd == "quit" || cmd == "exit") {
    BookStore_ZYM::shut_down = true;
    return std::vector<std::string>();
  }
  return std::vector<std::string>({cmd});
}