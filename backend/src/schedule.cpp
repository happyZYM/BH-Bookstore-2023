#include "schedule.h"
#include "engine.h"

void BookStoreBackEndClass::Run()
{
  std::string request_data;
  while(true)
  {
    input_ptr->getline(request_data, '\n');
    // std::string response_data = Engine::Process(request_data);
    // output_ptr << response_data << '\n';
  }
}