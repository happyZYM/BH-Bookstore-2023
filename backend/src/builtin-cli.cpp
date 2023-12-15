#include "builtin-cli.h"

#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>

#include "bs-utility.h"
#include "engine.h"
void BookStoreMain(bool is_server, std::string config_dir) {
  BookStoreEngineClass engine(config_dir, is_server);
  std::ios::sync_with_stdio(false);
  if (!is_server) {
    std::stack<std::pair<std::string, int>> login_stack;
    std::string cmd;
    while (std::getline(std::cin, cmd)) {
      auto result = std::move(engine.Execute(cmd, login_stack));
      // debugPrint(cmd);
      for (auto &line : result) {
        std::cout << line << '\n';
        // debugPrint(line);
      }
      // debugPrint();
      if (BookStore_ZYM::shut_down) return;
    }
  } else {
    // throw FatalError("Server mode has not been implemented yet", 1);
    std::unordered_map<std::string, SessionClass> session_map;
    std::string cmd;
    std::ofstream fout("/tmp/log.txt");
    while (std::getline(std::cin, cmd)) {
      fout << cmd << std::endl;
      fout.flush();
      if (cmd[1] == 'O')  //`#OpenSession [TempChannelID]`
      {
        std::string new_session_token = GenerateRandomString(10);
        std::string new_outh_token = GenerateRandomString(16);
        session_map[new_session_token] =
            SessionClass(new_session_token, new_outh_token);
        std::stringstream ss(cmd);
        std::string temp_channel_id;
        ss >> temp_channel_id;
        ss >> temp_channel_id;
        std::cout << temp_channel_id << " Init 1\n"
                  << new_session_token << ' ' << new_outh_token << std::endl;
        std::cout.flush();
      } else if (cmd[1] == 'S')
        return;
      else if (cmd[1] == 'C') {
        std::stringstream ss(cmd);
        std::string session_token, operation_token, authentic_key;
        ss >> session_token;
        ss >> session_token >> operation_token >> authentic_key;
        if (session_map.find(session_token) == session_map.end()) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        if (session_map[session_token].OuthorizationKey != authentic_key) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        session_map.erase(session_token);
        std::cout << session_token << ' ' << operation_token << " 0"
                  << std::endl;
        std::cout.flush();
      } else if (cmd[1] == 'W') {
        std::stringstream ss(cmd);
        std::string session_token, operation_token, authentic_key;
        ss >> session_token;
        ss >> session_token >> operation_token >> authentic_key;
        if (session_map.find(session_token) == session_map.end()) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        if (session_map[session_token].OuthorizationKey != authentic_key) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        if (session_map[session_token].login_stack.size()) {
          std::cout << session_token << ' ' << operation_token << " 1\n"
                    << engine.QueryUserInfo(
                           session_map[session_token].login_stack.top().first)
                    << std::endl;
          std::cout.flush();
        } else {
          std::cout << session_token << ' ' << operation_token
                    << " 1\n[nobody] 0" << std::endl;
          std::cout.flush();
        }
      } else if (cmd[1] == 'R') {
        std::stringstream ss(cmd);
        std::string session_token, operation_token, authentic_key;
        ss >> session_token;
        ss >> session_token >> operation_token >> authentic_key;
        if (session_map.find(session_token) == session_map.end()) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        if (session_map[session_token].OuthorizationKey != authentic_key) {
          std::cout << session_token << ' ' << operation_token << " -1"
                    << std::endl;
          std::cout.flush();
          continue;
        }
        std::getline(std::cin, cmd);
        auto ret = std::move(
            engine.Execute(cmd, session_map[session_token].login_stack));
        std::cout << session_token << ' ' << operation_token << " "
                  << ret.size() << std::endl;
        for (auto &line : ret) std::cout << line << std::endl;
        std::cout.flush();
      }
    }
  }
}