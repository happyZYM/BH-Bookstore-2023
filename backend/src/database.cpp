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