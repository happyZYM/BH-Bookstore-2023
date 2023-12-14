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
#include <vector>

namespace BookStore_ZYM {
extern std::mutex debug_Print_Mutex;
extern bool shut_down;
}  // namespace BookStore_ZYM

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
  void readlock();
  void unreadlock();
  std::atomic<bool> is_writing = false;

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
  condition.notify_all();
  return *this;
}

// Implementation of operator>>
template <typename T>
BlockingStringStream &BlockingStringStream::operator>>(T &val) {
  std::unique_lock<std::mutex> lock(mutex);

  // Wait until data is available
  condition.wait(lock, [this] {
    return internalStream.peek() != EOF && !is_writing;
  });

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
  std::stack<std::string> login_stack;
  std::string SessionToken;
  std::string OuthorizationKey;
};

void debugPrint();
template <typename... Args>
void debugPrint(Args... args) {
  BookStore_ZYM::debug_Print_Mutex.lock();
  ((std::cerr << args), ...);
  std::cerr << std::endl;
  BookStore_ZYM::debug_Print_Mutex.unlock();
}

void Respond(BlockingStringStream *output_ptr, std::string SessionToken,
             std::string OperationToken, std::string AuthenticationKey,
             const std::vector<std::string> &ret);

class UserItemClass {
 public:
  char user_id[31], password[31], user_name[31];
  unsigned char privilege;
};

class BookItemClass {
 public:
  char ISBN[21], name[61], author[61], keyword[61];
  double price;
  int quantity_remain;
  int bid;
};

class FinanceItemClass {
 public:
  int book_id;
  int quantity;
  double total_price;
};

class OperationLogItemClass {
 public:
  char user_id[31];
  char command[256];
  int fid;
};

enum OperationType {
  __Ksu,
  __Klogout,
  __Kregister,
  __Kpasswd,
  __Kuseradd,
  __Kdelete,
  __Kshow,
  __Kbuy,
  __Kselect,
  __Kmodify,
  __Kimport,
  __Kshowfinance,
  __Klog,
  __Kreport,
};
#endif  // PROTECTOR_UTILITY_H