#include "builtin-cli.h"

#include <iostream>

#include "bs-utility.h"
#include "schedule.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    BlockingStringStream input;
    BlockingStringStream output;
    BookStoreBackEndClass backend(config_dir, &input, &output);
    std::thread backend_thread([&backend]() { backend.Run(); });
    input << "#OpenSession INNERCLI\n";
    std::string SessionToken, AuthenticationKey, tmp;
    output.getline(tmp);
    output >> SessionToken >> AuthenticationKey;
    debugPrint("SessionToken=", SessionToken,
               " AuthenticationKey=", AuthenticationKey);
    std::string cmd;
    while (getline(std::cin, cmd)) {
      if (cmd == "quit" || cmd == "exit") {
        input << "#CloseSession " << SessionToken << ' ' << AuthenticationKey
              << '\n';
        input << "#ShutDownSystem\n";
        backend_thread.join();
        return;
      }
      input << "#Request " << SessionToken << " I-T-D " << AuthenticationKey
            << ' ' << cmd << '\n';
      std::string SessionToken;
      std::string OperationToken;
      int LineCounter;
      continue;
      output >> SessionToken >> OperationToken >> LineCounter;
      for (int i = 0; i < LineCounter; i++) {
        output.getline(tmp);
        std::cout << tmp << std::endl;
      }
    }
    input << "#CloseSession " << SessionToken << ' ' << AuthenticationKey
          << '\n';
    input << "#ShutDownSystem\n";
    backend_thread.join();
    return;
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