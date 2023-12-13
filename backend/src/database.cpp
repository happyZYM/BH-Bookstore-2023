#include "database.h"

void UserDataBase::Open(std::string file_name) {
  full_user_data.OpenFile(file_name + ".full");
  user_name2index.OpenFile(file_name + ".n2i");
}

void BookDataBase::Open(std::string file_name) {
  full_book_data.OpenFile(file_name + ".full");
  indexer.OpenFile(file_name + ".n2i");
}

void LogDataBase::Open(std::string file_name) {
  finance_data.OpenFile(file_name + ".finance");
  operation_log_data.OpenFile(file_name + ".log");
}

bool UserDataBase::PAM(const std::string &user_id,
                       const std::string &password) {
  auto ret = user_name2index.Find(user_id);
  if (ret.size() != 1) return false;
  UserItemClass tmp;
  full_user_data.read(tmp, ret[0]);
  return tmp.password == password;
}

int UserDataBase::GetPrevilege(const std::string &user_id) {
  auto ret = user_name2index.Find(user_id);
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