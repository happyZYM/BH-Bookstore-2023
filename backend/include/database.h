#ifndef PROTECTOR_DATABASE_HPP
#define PROTECTOR_DATABASE_HPP
#include <string>
#include <vector>

#include "bs-utility.h"
#include "drivearray.hpp"
#include "key2index.hpp"

class UserDataBase {
  DriveArray<UserItemClass> full_user_data;
  String2Index user_name2index;

 public:
  UserDataBase() = default;
  void Open(std::string file_name);
  bool PAM(const std::string &user_id, const std::string &password);
};
class BookDataBase {
  DriveArray<BookItemClass> full_book_data;
  String2Index indexer;

 public:
  void Open(std::string file_name);
};
class LogDataBase {
  DriveArray<FinanceItemClass> finance_data;
  DriveArray<OperationLogItemClass> operation_log_data;

 public:
  void Open(std::string file_name);
};

#endif  // PROTECTOR_DATABASE_HPP