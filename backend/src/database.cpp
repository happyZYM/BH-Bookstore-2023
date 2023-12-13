#include "database.h"

#include "bs-utility.h"
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

void BookDataBase::CreateEmptyBook(const std::string &ISBN) {
  BookItemClass tmp;
  strcpy(tmp.ISBN, ISBN.c_str());
  tmp.quantity_remain = tmp.price = 0;
  tmp.name[0] = 0;
  tmp.author[0] = 0;
  tmp.keyword[0] = 0;
  int idx = full_book_data.write(tmp);
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
    keyword2index.Delete(tmp.keyword, ret[0]);
    keyword2index.Insert(keyword, ret[0]);
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
  if (ISBN == "" && name == "" && author == "" && keyword == "") return;
}