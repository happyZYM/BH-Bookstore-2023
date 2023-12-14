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
  String2Index ISBN2index, name2index, author2index, keyword2index;

 public:
  DriveArray<BookItemClass> full_book_data;
  void Open(std::string file_name);
  bool HaveISBN(const std::string &ISBN);
  bool HaveISBN(const std::string &ISBN, BookItemClass &ret);
  void CreateEmptyBook(const std::string &ISBN);
  void QueryBook(const std::string &ISBN, const std::string &name,
                 const std::string &author, const std::string &keyword,
                 std::vector<BookItemClass> &ret);
  void ModifyInfo(const std::string &ISBN, const std::string &new_ISBN,
                  const std::string &name, const std::string &author,
                  const std::string &keyword, double price,
                  int quantity_remain);
  std::string GetISBN(int bid);
};
class LogDataBase {
  DriveArray<FinanceItemClass> finance_data;
  DriveArray<OperationLogItemClass> operation_log_data;
  int finance_operation_count;

 public:
  ~LogDataBase() { finance_data.write_info(finance_operation_count, 1); }
  void Open(std::string file_name);
  void AddSell(int book_id, int quantity, double total_price);
  void AddImport(int book_id, int quantity, double total_price);
  std::pair<double, double> QueryFinance(int count = -1);
  void GenaerateFinanceReport(std::string file_name,
                              std::vector<std::string> &ret,
                              BookDataBase &book_data);
  int TotalFinanceOperationCount() noexcept { return finance_operation_count; }
};

#endif  // PROTECTOR_DATABASE_HPP