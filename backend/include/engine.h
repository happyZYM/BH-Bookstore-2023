#ifndef PROTECTOR_ENGINE_H
#define PROTECTOR_ENGINE_H
#include <stack>
#include <string>
#include <vector>
class BookStoreEngineClass {
  std::string config_dir;

 public:
  BookStoreEngineClass() = delete;
  BookStoreEngineClass(std::string config_dir) : config_dir(config_dir) {}
  std::vector<std::string> Execute(const std::string &cmd, std::stack<std::string> &login_stack);
};
#endif  // PROTECTOR_ENGINE_H