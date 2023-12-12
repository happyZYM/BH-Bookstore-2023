#ifndef PROTECTOR_UTILITY_H
#define PROTECTOR_UTILITY_H
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <sstream>
#include <stack>
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
  void lock();
  void unlock();

 private:
  std::mutex mutex;
  std::mutex custom_mutex;
  std::condition_variable condition;
};
// Implementation of operator<<
template <typename T>
BlockingStringStream &BlockingStringStream::operator<<(const T &val) {
  {
    std::lock_guard<std::mutex> lock(mutex);
    if (internalStream.peek() == EOF) {
      internalStream.clear();
      internalStream.str("");
    }
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
class ReadWriteLock {
 private:
  std::mutex mtx;
  std::condition_variable cv;
  std::atomic<int> readers;
  std::atomic<bool> is_writing;

 public:
  ReadWriteLock();
  void startRead();
  void endRead();
  void startWrite();
  void endWrite();
};
class SessionClass {
 public:
  std::stack<int> login_stack;
  std::string SessionToken;
  std::string OuthorizationKey;
};
namespace BookStore_ZYM {
extern std::mutex debug_Print_Mutex;
}
void debugPrint();
template <typename... Args>
void debugPrint(Args... args) {
  BookStore_ZYM::debug_Print_Mutex.lock();
  ((std::cerr << args), ...);
  std::cerr << std::endl;
  BookStore_ZYM::debug_Print_Mutex.unlock();
}
#endif  // PROTECTOR_UTILITY_H