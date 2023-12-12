#ifndef PROTECTOR_ENGINE_H
#define PROTECTOR_ENGINE_H
#include <string>
class BookStoreEngineClass {
  std::string config_dir;

 public:
  BookStoreEngineClass() = delete;
  BookStoreEngineClass(std::string config_dir) : config_dir(config_dir) {}
};
#endif  // PROTECTOR_ENGINE_H