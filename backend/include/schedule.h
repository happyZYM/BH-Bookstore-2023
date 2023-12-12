#ifndef PROTECTOR_SCHEDULE_H
#define PROTECTOR_SCHEDULE_H
#include <string>

#include "bs-utility.h"
#include "engine.h"
class BookStoreBackEndClass {
  std::string config_dir;
  BlockingStringStream *input_ptr;
  BlockingStringStream *output_ptr;
  BookStoreEngineClass *engine_ptr;

 public:
  BookStoreBackEndClass() = delete;
  BookStoreBackEndClass(std::string config_dir, BlockingStringStream *input_ptr,
                        BlockingStringStream *output_ptr)
      : config_dir(config_dir), input_ptr(input_ptr), output_ptr(output_ptr) {
    engine_ptr = new BookStoreEngineClass(config_dir);
  }
  ~BookStoreBackEndClass() { delete engine_ptr; }
  void Run();
};
#endif  // PROTECTOR_SCHEDULE_H