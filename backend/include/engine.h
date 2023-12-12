#ifndef PROTECTOR_ENGINE_H
#define PROTECTOR_ENGINE_H
#include <stack>
#include <string>
#include <vector>

#include "database.h"
class BookStoreEngineClass {
  std::string config_dir;
  UserDataBase user_data_base;

 public:
  BookStoreEngineClass() = delete;
  BookStoreEngineClass(std::string __config_dir);
  std::vector<std::string> Execute(const std::string &cmd,
                                   std::stack<std::string> &login_stack);
};
#endif  // PROTECTOR_ENGINE_H