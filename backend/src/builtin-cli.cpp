#include "builtin-cli.h"

#include <cassert>
#include <iostream>

#include "bs-utility.h"
#include "schedule.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    int cnt = 0;
    BlockingStringStream input;
    BlockingStringStream output;
    BookStoreBackEndClass backend(config_dir, &input, &output);
    std::thread backend_thread([&backend]() { backend.Run(); });
    input.readlock();
    input << "#OpenSession INNERCLI\n";
    input.unreadlock();
    std::string SessionToken, AuthenticationKey, tmp;
    output.getline(tmp);
    output >> SessionToken >> AuthenticationKey;
    // debugPrint("SessionToken=", SessionToken,
    //  " AuthenticationKey=", AuthenticationKey);
    std::string cmd;
    output.getline(tmp);
    while (getline(std::cin, cmd)) {
      if (cmd == "quit" || cmd == "exit") {
        input.readlock();
        input << "#CloseSession " << SessionToken << ' ' << AuthenticationKey
              << '\n';
        input << "#ShutDownSystem\n";
        input.unreadlock();
        backend_thread.join();
        return;
      }
      input.readlock();
      input << "#Request " << SessionToken << " I-T-D" << ++cnt << " "
            << AuthenticationKey << ' ' << cmd << '\n';
      assert(input.internalStream.peek() != EOF);
      input.unreadlock();
      assert(input.is_writing == false);
      debugPrint("Sent Request ", cnt, " cmd=", cmd);
      std::string SessionToken;
      std::string OperationToken;
      int LineCounter;
      output >> SessionToken >> OperationToken >> LineCounter;
      debugPrint("Get the Head of response id=", OperationToken,
                 " LineCounter=", LineCounter);
      // debugPrint("Get SessionToken=", SessionToken,
      //            " OperationToken=", OperationToken,
      //            " LineCounter=", LineCounter);
      output.getline(tmp);
      for (int i = 0; i < LineCounter; i++) {
        output.getline(tmp);
        std::cout << tmp << std::endl;
        // std::cerr << tmp << std::endl;
      }
      std::cout.flush();
    }
    input.readlock();
    input << "#CloseSession " << SessionToken << ' ' << AuthenticationKey
          << '\n';
    input << "#ShutDownSystem\n";
    input.unreadlock();
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
        input.readlock();
        input << data << '\n';
        input.unreadlock();
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