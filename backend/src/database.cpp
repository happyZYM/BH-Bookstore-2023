#include "database.h"

#include <iomanip>
#include <sstream>
#include <unordered_map>

#include "bs-utility.h"
#include "lexer.h"
void UserDataBase::Open(std::string file_name) {
  full_user_data.OpenFile(file_name + ".full");
  user_name2index.OpenFile(file_name + ".n2i");
}

void BookDataBase::Open(std::string file_name) {
  full_book_data.OpenFile(file_name + ".full");
  ISBN2index.OpenFile(file_name + ".isbn");
  name2index.OpenFile(file_name + ".name");
  author2index.OpenFile(file_name + ".author");
  keyword2index.OpenFile(file_name + ".keyword");
}

void LogDataBase::Open(std::string file_name) {
  finance_data.OpenFile(file_name + ".finance");
  finance_data.get_info(finance_operation_count, 1);
  operation_log_data.OpenFile(file_name + ".log");
}

bool UserDataBase::PAM(const std::string &user_id,
                       const std::string &password) {
  // debugPrint("PAM ", user_id, " ", password);
  auto ret = user_name2index.Find(user_id);
  if (ret.size() != 1) return false;
  UserItemClass tmp;
  full_user_data.read(tmp, ret[0]);
  // debugPrint("Correct password: ", tmp.password, " Input password: ",
  // password);
  return tmp.password == password;
}

int UserDataBase::GetPrevilege(const std::string &user_id) {
  auto ret = user_name2index.Find(user_id);
  // debugPrint("size=", ret.size());
  if (ret.size() != 1) return -1;
  UserItemClass tmp;
  full_user_data.read(tmp, ret[0]);
  return tmp.privilege;
}

void UserDataBase::AddUser(const std::string &user_id,
                           const std::string &password,
                           const std::string &user_name, int privilege) {
  UserItemClass tmp;
  strcpy(tmp.user_id, user_id.c_str());
  strcpy(tmp.password, password.c_str());
  strcpy(tmp.user_name, user_name.c_str());
  tmp.privilege = privilege;
  int idx = full_user_data.write(tmp);
  user_name2index.Insert(user_id, idx);
  // debugPrint("Add user: ", user_id, " ", password, " ", user_name, " ",
  //            privilege);
  // debugPrint("idx: ", idx);
}

void UserDataBase::DeleteUser(const std::string &user_id) {
  auto ret = user_name2index.Find(user_id);
  if (ret.size() != 1) return;
  full_user_data.Delete(ret[0]);
  user_name2index.Delete(user_id, ret[0]);
}

void UserDataBase::ChangePassword(const std::string &user_id,
                                  const std::string &password) {
  auto ret = user_name2index.Find(user_id);
  if (ret.size() != 1) return;
  UserItemClass tmp;
  full_user_data.read(tmp, ret[0]);
  strcpy(tmp.password, password.c_str());
  full_user_data.update(tmp, ret[0]);
}

bool BookDataBase::HaveISBN(const std::string &ISBN) {
  auto ret = ISBN2index.Find(ISBN);
  return ret.size() == 1;
}
bool BookDataBase::HaveISBN(const std::string &ISBN, BookItemClass &ans) {
  auto ret = ISBN2index.Find(ISBN);
  if (ret.size() == 1) {
    full_book_data.read(ans, ret[0]);
    return true;
  }
  return false;
}

void BookDataBase::CreateEmptyBook(const std::string &ISBN) {
  BookItemClass tmp;
  strcpy(tmp.ISBN, ISBN.c_str());
  tmp.quantity_remain = tmp.price = 0;
  tmp.name[0] = 0;
  tmp.author[0] = 0;
  tmp.keyword[0] = 0;
  int idx = full_book_data.write(tmp);
  tmp.bid = idx;
  // debugPrint("CreateEmptyBook: ", ISBN, " ", idx);
  full_book_data.update(tmp, idx);
  ISBN2index.Insert(ISBN, idx);
  name2index.Insert("", idx);
  author2index.Insert("", idx);
  keyword2index.Insert("", idx);
}

void BookDataBase::ModifyInfo(const std::string &ISBN,
                              const std::string &new_ISBN,
                              const std::string &name,
                              const std::string &author,
                              const std::string &keyword, double price,
                              int quantity_remain) {
  auto ret = ISBN2index.Find(ISBN);
  if (ret.size() != 1) return;
  BookItemClass tmp;
  full_book_data.read(tmp, ret[0]);
  if (new_ISBN != "") {
    ISBN2index.Delete(ISBN, ret[0]);
    ISBN2index.Insert(new_ISBN, ret[0]);
    strcpy(tmp.ISBN, new_ISBN.c_str());
  }
  if (name != "") {
    name2index.Delete(tmp.name, ret[0]);
    name2index.Insert(name, ret[0]);
    strcpy(tmp.name, name.c_str());
  }
  if (author != "") {
    author2index.Delete(tmp.author, ret[0]);
    author2index.Insert(author, ret[0]);
    strcpy(tmp.author, author.c_str());
  }
  if (keyword != "") {
    std::vector<std::string> keyword_vec;
    KeyWordSpliter(tmp.keyword, keyword_vec, true);
    for (auto &i : keyword_vec) {
      keyword2index.Delete(i, ret[0]);
    }
    KeyWordSpliter(keyword, keyword_vec);
    for (auto &i : keyword_vec) {
      keyword2index.Insert(i, ret[0]);
    }
    strcpy(tmp.keyword, keyword.c_str());
  }
  if (price >= 0) tmp.price = price;
  if (quantity_remain >= 0) tmp.quantity_remain = quantity_remain;
  full_book_data.update(tmp, ret[0]);
}

void BookDataBase::QueryBook(const std::string &ISBN, const std::string &name,
                             const std::string &author,
                             const std::string &keyword,
                             std::vector<BookItemClass> &ret) {
  ret.clear();
  std::vector<BookItemClass> cache;
  std::vector<unsigned char> valid;
  if (ISBN == "" && name == "" && author == "" && keyword == "") {
    full_book_data.FetchAll(ret);
    /* sort by ISBN */
    sort(ret.begin(), ret.end(),
         [](const BookItemClass &a, const BookItemClass &b) {
           return strcmp(a.ISBN, b.ISBN) < 0;
         });
    return;
  }
  bool first_ristrict = true;
  if (ISBN != "") {
    first_ristrict = false;
    auto tmp = ISBN2index.Find(ISBN);
    if (tmp.size() == 1) {
      BookItemClass tmp_book;
      full_book_data.read(tmp_book, tmp[0]);
      cache.push_back(tmp_book);
      valid.push_back(1);
    }
    if (tmp.size() > 1) throw FatalError("ISBN not unique", 7);
  }
  if (name != "") {
    if (first_ristrict) {
      first_ristrict = false;
      auto tmp = name2index.Find(name);
      for (auto &i : tmp) {
        BookItemClass tmp_book;
        full_book_data.read(tmp_book, i);
        cache.push_back(tmp_book);
        valid.push_back(1);
      }
    } else {
      for (int i = 0; i < cache.size(); i++) {
        if (!valid[i]) continue;
        if (strcmp(cache[i].name, name.c_str()) != 0) {
          valid[i] = 0;
        }
      }
    }
  }
  if (author != "") {
    if (first_ristrict) {
      first_ristrict = false;
      auto tmp = author2index.Find(author);
      for (auto &i : tmp) {
        BookItemClass tmp_book;
        full_book_data.read(tmp_book, i);
        cache.push_back(tmp_book);
        valid.push_back(1);
      }
    } else {
      for (int i = 0; i < cache.size(); i++) {
        if (!valid[i]) continue;
        if (strcmp(cache[i].author, author.c_str()) != 0) {
          valid[i] = 0;
        }
      }
    }
  }
  if (keyword != "") {
    if (first_ristrict) {
      first_ristrict = false;
      auto tmp = keyword2index.Find(keyword);
      for (auto &j : tmp) {
        BookItemClass tmp_book;
        full_book_data.read(tmp_book, j);
        cache.push_back(tmp_book);
        valid.push_back(1);
      }
    } else {
      for (int i = 0; i < cache.size(); i++) {
        if (!valid[i]) continue;
        if (strcmp(cache[i].keyword, keyword.c_str()) != 0) {
          valid[i] = 0;
        }
      }
    }
  }
  for (int i = 0; i < cache.size(); i++) {
    if (valid[i]) ret.push_back(cache[i]);
  }
  sort(ret.begin(), ret.end(),
       [](const BookItemClass &a, const BookItemClass &b) {
         return strcmp(a.ISBN, b.ISBN) < 0;
       });
}

void LogDataBase::AddImport(int book_id, int quantity, double total_price) {
  FinanceItemClass tmp;
  tmp.book_id = book_id;
  tmp.quantity = quantity;
  tmp.total_price = -total_price;
  finance_data.write(tmp);
  finance_operation_count++;
}

void LogDataBase::AddSell(int book_id, int quantity, double total_price) {
  FinanceItemClass tmp;
  tmp.book_id = book_id;
  tmp.quantity = quantity;
  tmp.total_price = total_price;
  finance_data.write(tmp);
  finance_operation_count++;
}

std::pair<double, double> LogDataBase::QueryFinance(int count) {
  double total_sell = 0, total_import = 0;
  if (count == -1) count = finance_operation_count;
  for (int i = 1; i <= count; i++) {
    FinanceItemClass tmp;
    finance_data.read(tmp, finance_operation_count + 1 - i);
    if (tmp.total_price > 0)
      total_sell += tmp.total_price;
    else
      total_import -= tmp.total_price;
  }
  return std::make_pair(total_sell, total_import);
}

void LogDataBase::GenaerateFinanceReport(std::string file_name,
                                         std::vector<std::string> &ret,
                                         BookDataBase &book_data) {
  ret.clear();
  std::unordered_map<int, std::pair<double, double>>
      bucket;  // classify by book_id
  for (int i = 1; i <= finance_operation_count; i++) {
    FinanceItemClass tmp;
    finance_data.read(tmp, i);
    if (tmp.total_price > 0) {
      bucket[tmp.book_id].first += tmp.total_price;
    } else {
      bucket[tmp.book_id].second -= tmp.total_price;
    }
  }
  std::vector<std::pair<int, std::pair<double, double>>> tmp;
  for (auto &i : bucket) {
    tmp.push_back(i);
  }
  // first, generate a table sorted by sales
  sort(tmp.begin(), tmp.end(),
       [](const std::pair<int, std::pair<double, double>> &a,
          const std::pair<int, std::pair<double, double>> &b) {
         return a.second.first > b.second.first;
       });
  ret.push_back("Sale Ranking:");
  /**
   * The format of each line is:
   * [ISBN]\t[Name]\t+[Sales]\t-[Cost]
   *
   * note that Sales and Profit should have a precision of 2
   */
  for (auto &i : tmp) {
    BookItemClass tmp_book;
    book_data.full_book_data.read(tmp_book, i.first);
    std::stringstream ss;
    ss << "+" << std::fixed << std::setprecision(2) << i.second.first << "\t-"
       << std::fixed << std::setprecision(2) << i.second.second;
    ret.push_back(std::string(tmp_book.ISBN) + "\t" +
                  std::string(tmp_book.name) + "\t" + ss.str());
  }
  // then, generate a table sorted by cost
  sort(tmp.begin(), tmp.end(),
       [](const std::pair<int, std::pair<double, double>> &a,
          const std::pair<int, std::pair<double, double>> &b) {
         return a.second.second > b.second.second;
       });
  ret.push_back("--------------------");
  ret.push_back("Cost Ranking:");
  /**
   * The format of each line is:
   * [ISBN]\t[Name]\t-[Cost]\t+[Sales]
   *
   * note that Cost and Profit should have a precision of 2
   */
  for (auto &i : tmp) {
    BookItemClass tmp_book;
    book_data.full_book_data.read(tmp_book, i.first);
    std::stringstream ss;
    ss << "-" << std::fixed << std::setprecision(2) << i.second.second << "\t+"
       << std::fixed << std::setprecision(2) << i.second.first;
    ret.push_back(std::string(tmp_book.ISBN) + "\t" +
                  std::string(tmp_book.name) + "\t" + ss.str());
  }
}

std::string BookDataBase::GetISBN(int bid) {
  BookItemClass tmp;
  full_book_data.read(tmp, bid);
  return std::string(tmp.ISBN);
}