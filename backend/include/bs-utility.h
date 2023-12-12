#ifndef PROTECTOR_UTILITY_H
#define PROTECTOR_UTILITY_H
#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
class FatalError : public std::exception {
 public:
  FatalError(const char *__message, int __code)
      : message(__message), code(__code) {}

  const char *what() const noexcept override { return message.c_str(); }
  const int GetCode() const noexcept { return code; }

 private:
  std::string message;
  int code;
};
class BlockingStringStream {
 public:
  BlockingStringStream() {}

  // Declaration of major std::stringstream interfaces
  template <typename T>
  BlockingStringStream &operator<<(const T &val);

  template <typename T>
  BlockingStringStream &operator>>(T &val);
  BlockingStringStream &getline(std::string &str, char delim = '\n');
  std::stringstream internalStream;

 private:
  std::mutex mutex;
  std::condition_variable condition;
};
// Implementation of operator<<
template <typename T>
BlockingStringStream &BlockingStringStream::operator<<(const T &val) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (internalStream.peek() == EOF) internalStream.clear();
    internalStream << val;
  }
  condition.notify_one();
  return *this;
}

// Implementation of operator>>
template <typename T>
BlockingStringStream &BlockingStringStream::operator>>(T &val) {
  std::unique_lock<std::mutex> lock(mutex);

  // Wait until data is available
  condition.wait(lock, [this] { return internalStream.peek() != EOF; });

  internalStream >> val;

  return *this;
}
#endif  // PROTECTOR_UTILITY_H