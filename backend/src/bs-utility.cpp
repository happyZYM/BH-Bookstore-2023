#include "bs-utility.h"

BlockingStringStream &BlockingStringStream::getline(std::string &str,
                                                    char delim) {
  std::unique_lock<std::mutex> lock(mutex);

  // Wait until data is available
  condition.wait(lock, [this] { return internalStream.peek() != EOF; });

  std::getline(internalStream, str, delim);

  return *this;
}