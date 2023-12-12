#include "database.h"

void UserDataBase::Open(std::string file_name) {
  full_user_data.OpenFile(file_name + ".full");
  user_name2index.OpenFile(file_name + ".n2i");
}