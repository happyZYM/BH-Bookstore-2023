#include "bs-utility.h"

#include <random>
namespace BookStore_ZYM {
std::mutex debug_Print_Mutex;
bool shut_down = false;
}  // namespace BookStore_ZYM

BlockingStringStream &BlockingStringStream::getline(std::string &str,
                                                    char delim) {
  std::unique_lock<std::mutex> lock(mutex);

  // Wait until data is available
  condition.wait(lock, [this] {
    return internalStream.peek() != EOF && !is_writing;
  });
  str = "$FAILED$";
  std::getline(internalStream, str, delim);

  return *this;
}

ReadWriteLock::ReadWriteLock() : readers(0), is_writing(false) {}

void ReadWriteLock::startRead() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] { return !is_writing; });  // 等待写操作结束
  readers++;
}

void ReadWriteLock::endRead() {
  std::unique_lock<std::mutex> lock(mtx);
  readers--;
  if (readers == 0) {
    cv.notify_all();  // 唤醒一个等待的写操作
  }
}

void ReadWriteLock::startWrite() {
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait(lock, [this] {
    return readers == 0 && !is_writing;
  });  // 等待读操作和写操作结束
  is_writing = true;
}

void ReadWriteLock::endWrite() {
  std::unique_lock<std::mutex> lock(mtx);
  is_writing = false;
  cv.notify_all();  // 唤醒所有等待的读操作和写操作
}

void debugPrint() {
  BookStore_ZYM::debug_Print_Mutex.lock();
  std::cerr << std::endl;
  BookStore_ZYM::debug_Print_Mutex.unlock();
}

void BlockingStringStream::readlock() {
  mutex.lock();
  is_writing = true;
  mutex.unlock();
}
void BlockingStringStream::unreadlock() {
  mutex.lock();
  is_writing = false;
  mutex.unlock();
  condition.notify_all();
}

void Respond(BlockingStringStream *output_ptr, std::string SessionToken,
             std::string OperationToken, std::string AuthenticationKey,
             const std::vector<std::string> &ret) {
  static std::mutex output_mutex;
  output_mutex.lock();
  (*output_ptr).readlock();
  (*output_ptr) << SessionToken << ' ' << OperationToken << " " << ret.size()
                << '\n';
  for (int i = 0; i < ret.size(); i++) (*output_ptr) << ret[i] << '\n';
  (*output_ptr).unreadlock();
  output_mutex.unlock();
}

std::string GenerateRandomString(int length) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 61);
  std::string ret;
  for (int i = 0; i < length; i++) {
    int x = dis(gen);
    if (x < 10)
      ret += '0' + x;
    else if (x < 36)
      ret += 'a' + x - 10;
    else
      ret += 'A' + x - 36;
  }
  return ret;
}