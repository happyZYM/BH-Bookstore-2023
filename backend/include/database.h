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
  int GetPrevilege(const std::string &user_id);
  void AddUser(const std::string &user_id, const std::string &password,
               const std::string &user_name, int privilege);
  void DeleteUser(const std::string &user_id);
  void ChangePassword(const std::string &user_id, const std::string &password);
};
class BookDataBase {
  DriveArray<BookItemClass> full_book_data;
  String2Index ISBN2index, name2index, author2index, keyword2index;

 public:
  void Open(std::string file_name);
  bool HaveISBN(const std::string &ISBN);
  bool HaveISBN(const std::string &ISBN,BookItemClass &ret);
  void CreateEmptyBook(const std::string &ISBN);
  void QueryBook(const std::string &ISBN, const std::string &name,
                 const std::string &author, const std::string &keyword,
                 std::vector<BookItemClass> &ret);
  void ModifyInfo(const std::string &ISBN, const std::string &new_ISBN,
                  const std::string &name, const std::string &author,
                  const std::string &keyword, double price,
                  int quantity_remain);
};
class LogDataBase {
  DriveArray<FinanceItemClass> finance_data;
  DriveArray<OperationLogItemClass> operation_log_data;

 public:
  void Open(std::string file_name);
  void QueryFinance(int count, std::vector<FinanceItemClass> &ret);
};

#endif  // PROTECTOR_DATABASE_HPP