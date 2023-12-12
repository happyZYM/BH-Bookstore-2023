#include "schedule.h"

#include <cassert>
#include <iostream>
#include <random>
#include <sstream>

#include "engine.h"
void BookStoreBackEndClass::Run() {
  std::string request_data;
  const unsigned int RndSeed = std::random_device{}();
  std::mt19937 rnd(RndSeed);
  while (true) {
    input_ptr->getline(request_data, '\n');
    // debugPrint("Get_request_data=", request_data);
    if (request_data[1] == 'O')  // #OpenSession [TempChannelID]
    {
      std::stringstream ss;
      ss << request_data;
      ss >> request_data;
      std::string TempChannelID;
      ss >> TempChannelID;
      SessionClass new_session;
      std::string new_SessionToken;
      std::string new_AuthenticationKey;
      for (int i = 0; i < 16; i++) new_SessionToken.push_back(rnd() % 26 + 'A');
      for (int i = 0; i < 16; i++)
        new_AuthenticationKey.push_back(rnd() % 26 + 'A');
      new_session.SessionToken = new_SessionToken;
      new_session.OuthorizationKey = new_AuthenticationKey;
      session_map[new_SessionToken] = new_session;
      (*output_ptr).readlock();
      (*output_ptr) << TempChannelID << " IinitialOpt 1\n"
                    << new_SessionToken << ' ' << new_AuthenticationKey << '\n';
      (*output_ptr).unreadlock();
    } else if (request_data[1] == 'C') {
      ;
    } else if (request_data[1] == '_') {
      std::stringstream ss(request_data);
      std::string SessionToken;
      std::string OperationToken;
      std::string OuthenticationKey;
      std::string cmd;
      ss >> cmd >> SessionToken >> OperationToken >> OuthenticationKey;
      (*output_ptr).readlock();
      (*output_ptr) << SessionToken << ' ' << OperationToken << " 1\n"
                    << "[Internal Error] This API shouldn't be called\n";
      (*output_ptr).unreadlock();
    } else if (request_data[1] == 'S') {
      return;
    } else if (request_data[1] == 'R') {
      std::stringstream ss(request_data);
      std::string SessionToken;
      std::string OperationToken;
      std::string OuthenticationKey;
      std::string cmd;
      ss >> cmd >> SessionToken >> OperationToken >> OuthenticationKey;
      ss.get();
      std::getline(ss, cmd);
      PostRequest(SessionToken, OperationToken, OuthenticationKey, cmd);
    }
  }
}

void BookStoreBackEndClass::PostRequest(std::string SessionToken,
                                        std::string OperationToken,
                                        std::string AuthenticationKey,
                                        std::string cmd) {
  if (session_map[SessionToken].OuthorizationKey != AuthenticationKey) {
    Respond(output_ptr, SessionToken, OperationToken, AuthenticationKey,
            std::vector<std::string>({"[Error] AuthenticationKey is wrong"}));
    return;
  }
  Respond(output_ptr, SessionToken, OperationToken, AuthenticationKey,
          std::vector<std::string>({cmd}));
}