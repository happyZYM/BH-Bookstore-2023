#include "schedule.h"

#include <random>
#include <sstream>

#include "engine.h"
void BookStoreBackEndClass::Run() {
  std::string request_data;
  const unsigned int RndSeed = std::random_device{}();
  std::mt19937 rnd(RndSeed);
  while (true) {
    input_ptr->getline(request_data, '\n');
    debugPrint("request_data=", request_data);
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
      (*output_ptr) << TempChannelID << " IinitialOpt 1\n"
                    << new_SessionToken << ' ' << new_AuthenticationKey << '\n';
    }
  }
}