#include "bs-utility.h"

BlockingStringStream &BlockingStringStream::getline(std::string &str,
                                                    char delim) {
  std::unique_lock<std::mutex> lock(mutex);

  // Wait until data is available
  condition.wait(lock, [this] { return internalStream.peek() != EOF; });

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
namespace BookStore_ZYM {
std::mutex debug_Print_Mutex;
}
void debugPrint() {
  BookStore_ZYM::debug_Print_Mutex.lock();
  std::cerr << std::endl;
  BookStore_ZYM::debug_Print_Mutex.unlock();
}

void BlockingStringStream::lock() { custom_mutex.lock(); }
void BlockingStringStream::unlock() { custom_mutex.unlock(); }