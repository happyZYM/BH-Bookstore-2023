#include "builtin-cli.h"

#include <iostream>

#include "bs-utility.h"
#include "schedule.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    ;  // TODO: run as client
  } else {
    std::cin.tie(nullptr);
    std::cout.rdbuf(nullptr);
    BlockingStringStream input;
    BlockingStringStream output;
    BookStoreBackEndClass backend(config_dir, &input, &output);
    std::thread backend_thread([&backend]() { backend.Run(); });
    std::thread input_thread([&input]() {
      std::string data;
      while (std::getline(std::cin, data)) {
        input << data << '\n';
      }
    });
    std::thread output_thread([&output]() {
      std::string data;
      while (true) {
        output.getline(data, '\n');
        std::cout << data << std::endl;
      }
    });
    input_thread.join();
    output_thread.join();
    backend_thread.join();
  }
}