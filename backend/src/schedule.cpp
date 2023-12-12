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
      // assert((*output_ptr).internalStream.peek() != EOF);
      (*output_ptr).unreadlock();
      // assert((*output_ptr).is_writing == false);
      // debugPrint("Sent Response Init");
    } else if (request_data[1] == 'C') {
      ;
    } else if (request_data[1] == '_') {
      ;
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
      // debugPrint("SessionToken=", SessionToken,
      //            " OperationToken=", OperationToken,
      //            " OuthenticationKey=", OuthenticationKey, " cmd=", cmd);
      (*output_ptr).readlock();
      (*output_ptr) << SessionToken << ' ' << OperationToken << " 1\n"
                    << cmd << '\n';
      // assert((*output_ptr).internalStream.peek() != EOF);
      (*output_ptr).unreadlock();
      // assert((*output_ptr).is_writing == false);
      // debugPrint("Sent Response id=", OperationToken);
      // debugPrint(SessionToken, ' ', OperationToken, " 1\n", cmd);
    }
  }
}