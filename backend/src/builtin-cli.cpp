#include "builtin-cli.h"

#include <iostream>

#include "schedule.h"
#include "bs-utility.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    ;  // TODO: run as client
  } else {
    throw FatalError("Server mode is not implemented yet", 1);
    std::cin.tie(nullptr);
    std::cout.rdbuf(nullptr);
  }
}