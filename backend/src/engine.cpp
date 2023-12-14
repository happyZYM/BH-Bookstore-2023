#include "engine.h"

#include <cctype>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "bs-utility.h"

BookStoreEngineClass::BookStoreEngineClass(std::string __config_dir,
                                           bool __is_server) {
  config_dir = __config_dir;
  user_data_base.Open(config_dir + "user");
  book_data_base.Open(config_dir + "book");
  log_data_base.Open(config_dir + "log");
  is_server = __is_server;
  if (user_data_base.GetPrevilege("root") == -1) {
    // debugPrint("Creating root user");
    user_data_base.AddUser("root", "sjtu", "root", 7);
    // debugPrint("Now root's previlege is",
    // user_data_base.GetPrevilege("root"));
  }
}
std::vector<std::string> BookStoreEngineClass::Execute(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  for (int i = 0; i < cmd.length(); i++)
    if (std::isspace(cmd[i]) && cmd[i] != ' ')
      return std::vector<std::string>({"Invalid"});
  static std::unordered_map<std::string, OperationType> operation_map = {
      {"su", OperationType::__Ksu},
      {"logout", OperationType::__Klogout},
      {"useradd", OperationType::__Kuseradd},
      {"register", OperationType::__Kregister},
      {"delete", OperationType::__Kdelete},
      {"passwd", OperationType::__Kpasswd},
      {"select", OperationType::__Kselect},
      {"modify", OperationType::__Kmodify},
      {"import", OperationType::__Kimport},
      {"show", OperationType::__Kshow},
      {"buy", OperationType::__Kbuy},
      {"report", OperationType::__Kreport},
      {"log", OperationType::__Klog}};
  std::stringstream ss(cmd);
  std::string head = "";
  ss >> head;
  if (head == "quit" || head == "exit") {
    while (login_stack.size()) {
      login_count[login_stack.top().first]--;
      login_stack.pop();
    }
    if (!is_server) BookStore_ZYM::shut_down = true;
    return std::vector<std::string>();
  }
  if (operation_map.find(head) == operation_map.end()) {
    for (int i = 0; i < cmd.length(); i++)
      if (cmd[i] != ' ') return std::vector<std::string>({"Invalid"});
    return std::vector<std::string>();
  }
  switch (operation_map[head]) {
    case OperationType::__Ksu: {
      return ExecuteSu(cmd, login_stack);
    }
    case OperationType::__Klogout: {
      return ExecuteLogout(cmd, login_stack);
    }
    case OperationType::__Kuseradd: {
      return ExecuteUserAdd(cmd, login_stack);
    }
    case OperationType::__Kregister: {
      return ExecuteRegister(cmd, login_stack);
    }
    case OperationType::__Kdelete: {
      return ExecuteDelete(cmd, login_stack);
    }
    case OperationType::__Kpasswd: {
      return ExecutePasswd(cmd, login_stack);
    }
    case OperationType::__Kselect: {
      return ExecuteSelect(cmd, login_stack);
    }
    case OperationType::__Kmodify: {
      return ExecuteMOdify(cmd, login_stack);
    }
    case OperationType::__Kimport: {
      return ExecuteImport(cmd, login_stack);
    }
    case OperationType::__Kshow: {
      ss >> head;
      if (head == "finance") goto dst_showfinance;
      return ExecuteShow(cmd, login_stack);
    }
    case OperationType::__Kshowfinance: {
    dst_showfinance:;
      return ExecuteShowFinance(cmd, login_stack);
    }
    case OperationType::__Kbuy: {
      return ExecuteBuy(cmd, login_stack);
    }
    case OperationType::__Kreport: {
      // return std::vector<std::string>({"Invalid"});
      return ExecuteReport(cmd, login_stack);
    }
    case OperationType::__Klog: {
      // return std::vector<std::string>({"Invalid"});
      return ExecuteLog(cmd, login_stack);
    }
  }
  throw FatalError("Unknown Command", 5);
  return std::vector<std::string>({cmd});
}

std::vector<std::string> BookStoreEngineClass::ExecuteSu(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string user_id, password;
  if (!CommandSuLexer(cmd, user_id, password))
    return std::vector<std::string>({"Invalid"});
  // debugPrint("su", user_id, " ", password);
  if (login_stack.size() > 0 &&
      user_data_base.GetPrevilege(login_stack.top().first) == 7) {
    // debugPrint("has root previlege");
    if (user_data_base.GetPrevilege(user_id) == -1)
      return std::vector<std::string>({"Invalid"});
    login_stack.push(std::make_pair(user_id, 0));
    login_count[user_id]++;
    return std::vector<std::string>();
  }
  // debugPrint("Examining", user_id, password);
  if (user_data_base.PAM(user_id, password)) {
    login_stack.push(std::make_pair(user_id, 0));
    login_count[user_id]++;
    return std::vector<std::string>();
  }
  return std::vector<std::string>({"Invalid"});
}

std::vector<std::string> BookStoreEngineClass::ExecuteLogout(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  if (login_stack.empty()) return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(login_stack.top().first) < 1) {
    return std::vector<std::string>({"Invalid"});
  }
  login_count[login_stack.top().first]--;
  login_stack.pop();
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteRegister(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string user_id, password, user_name;
  if (!CommandRegisterLexer(cmd, user_id, password, user_name))
    return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(cmd) != -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.AddUser(user_id, password, user_name, 1);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecutePasswd(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string user_id, current_password, new_password;
  if (!CommandPasswdLexer(cmd, user_id, current_password, new_password))
    return std::vector<std::string>({"Invalid"});
  // debugPrint("sucessfully lexed passwd");
  if (user_data_base.GetPrevilege(user_id) < 1)
    return std::vector<std::string>({"Invalid"});
  // debugPrint("begin checing authority");
  if (login_stack.size() > 0 &&
      user_data_base.GetPrevilege(login_stack.top().first) == 7) {
    if (current_password != "") {
      if (!user_data_base.PAM(user_id, current_password))
        return std::vector<std::string>({"Invalid"});
    }
    user_data_base.ChangePassword(user_id, new_password);
    return std::vector<std::string>();
  }
  if (!user_data_base.PAM(user_id, current_password))
    return std::vector<std::string>({"Invalid"});
  user_data_base.ChangePassword(user_id, new_password);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteUserAdd(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  int own_previlege = 0;
  if (login_stack.size() > 0)
    own_previlege = user_data_base.GetPrevilege(login_stack.top().first);
  if (login_stack.empty() || own_previlege < 3)
    return std::vector<std::string>({"Invalid"});
  std::string user_id, password, user_name;
  int privilege;
  if (!CommandUseraddLexer(cmd, user_id, password, privilege, user_name))
    return std::vector<std::string>({"Invalid"});
  if (privilege >= own_previlege) return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(user_id) != -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.AddUser(user_id, password, user_name, privilege);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteDelete(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 7)
    return std::vector<std::string>({"Invalid"});
  std::string user_id;
  if (!CommandDeleteLexer(cmd, user_id))
    return std::vector<std::string>({"Invalid"});
  if (login_count[user_id] > 0) return std::vector<std::string>({"Invalid"});
  if (user_data_base.GetPrevilege(user_id) == -1)
    return std::vector<std::string>({"Invalid"});
  user_data_base.DeleteUser(user_id);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShow(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 1)
    return std::vector<std::string>({"Invalid"});
  std::string ISBN, name, author, keyword;
  if (!CommandShowLexer(cmd, ISBN, name, author, keyword))
    return std::vector<std::string>({"Invalid"});
  std::vector<BookItemClass> ret;
  book_data_base.QueryBook(ISBN, name, author, keyword, ret);
  if (ret.empty()) return std::vector<std::string>({""});
  /* output format
   * [ISBN]\t[BookName]\t[Author]\t[Keyword]\t[Price]\t[库存数量]\n */
  std::vector<std::string> ans;
  for (auto &i : ret) {
    std::string tmp = i.ISBN;
    tmp += '\t';
    tmp += i.name;
    tmp += '\t';
    tmp += i.author;
    tmp += '\t';
    tmp += i.keyword;
    tmp += '\t';
    unsigned long long price_tmp = i.price * 100 + 0.5;
    tmp += std::to_string(price_tmp / 100) + "." +
           std::to_string(price_tmp % 100 / 10) +
           std::to_string(price_tmp % 10);
    tmp += '\t';
    tmp += std::to_string(i.quantity_remain);
    ans.push_back(tmp);
  }
  return ans;
}

std::vector<std::string> BookStoreEngineClass::ExecuteBuy(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string ISBN;
  int quantity;
  if (!CommandBuyLexer(cmd, ISBN, quantity))
    return std::vector<std::string>({"Invalid"});
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 1)
    return std::vector<std::string>({"Invalid"});
  BookItemClass tmp;
  if (!book_data_base.HaveISBN(ISBN, tmp))
    return std::vector<std::string>({"Invalid"});
  if (quantity <= 0) return std::vector<std::string>({"Invalid"});
  if (quantity > tmp.quantity_remain)
    return std::vector<std::string>({"Invalid"});
  book_data_base.ModifyInfo(ISBN, "", "", "", "", -1,
                            tmp.quantity_remain - quantity);
  log_data_base.AddSell(tmp.bid, quantity, tmp.price * quantity);
  /*浮点数输出购买图书所需的总金额，小数点后有且仅有两位小数*/
  std::vector<std::string> ans;
  unsigned long long cost_tmp = tmp.price * quantity * 100 + 0.5;
  ans.push_back(std::to_string(cost_tmp / 100) + '.' +
                std::to_string(cost_tmp % 100 / 10) +
                std::to_string(cost_tmp % 10));
  return ans;
}

std::vector<std::string> BookStoreEngineClass::ExecuteSelect(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string ISBN;
  if (!CommandSelectLexer(cmd, ISBN))
    return std::vector<std::string>({"Invalid"});
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 3)
    return std::vector<std::string>({"Invalid"});
  BookItemClass BI_tmp;
  if (!book_data_base.HaveISBN(ISBN, BI_tmp)) {
    book_data_base.CreateEmptyBook(ISBN);
    book_data_base.HaveISBN(ISBN, BI_tmp);
  }
  std::pair<std::string, int> tmp;
  tmp = login_stack.top();
  login_stack.pop();
  tmp.second = BI_tmp.bid;
  // debugPrint("selected bid=", tmp.second);
  login_stack.push(tmp);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteMOdify(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  std::string new_ISBN, name, author, keyword;
  double price;
  if (!CommandModifyLexer(cmd, new_ISBN, name, author, keyword, price))
    return std::vector<std::string>({"Invalid"});
  // debugPrint("successfully lexed modify");
  // debugPrint("modify", new_ISBN, ' ', name, ' ', author, ' ', keyword, ' ',
  //  price);
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 3)
    return std::vector<std::string>({"Invalid"});
  // debugPrint("successfully checked authority");
  // debugPrint("selected book's bid=", login_stack.top().second);
  if (login_stack.top().second == 0 ||
      book_data_base.GetISBN(login_stack.top().second) == new_ISBN)
    return std::vector<std::string>({"Invalid"});
  // debugPrint("successfully checked ISBN");
  if (new_ISBN != "" && book_data_base.HaveISBN(new_ISBN))
    return std::vector<std::string>({"Invalid"});
  if (keyword != "") {
    std::vector<std::string> key_list;
    if (!KeyWordSpliter(keyword, key_list, false))
      return std::vector<std::string>({"Invalid"});
    std::unordered_set<std::string> key_set;
    for (auto &i : key_list) {
      if (key_set.find(i) != key_set.end())
        return std::vector<std::string>({"Invalid"});
      key_set.insert(i);
    }
  }
  // debugPrint("successfully checked keyword");
  book_data_base.ModifyInfo(book_data_base.GetISBN(login_stack.top().second),
                            new_ISBN, name, author, keyword, price, -1);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteImport(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  int quantity;
  double total_cost;
  if (!CommandImportLexer(cmd, quantity, total_cost))
    return std::vector<std::string>({"Invalid"});
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 3)
    return std::vector<std::string>({"Invalid"});
  if (login_stack.top().second == 0)
    return std::vector<std::string>({"Invalid"});
  if (quantity <= 0) return std::vector<std::string>({"Invalid"});
  if (total_cost <= 0) return std::vector<std::string>({"Invalid"});
  BookItemClass tmp;
  book_data_base.HaveISBN(book_data_base.GetISBN(login_stack.top().second),
                          tmp);
  book_data_base.ModifyInfo(book_data_base.GetISBN(login_stack.top().second),
                            "", "", "", "", -1, tmp.quantity_remain + quantity);
  log_data_base.AddImport(tmp.bid, quantity, total_cost);
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteShowFinance(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  int count;
  if (!CommandShowfinanceLexer(cmd, count))
    return std::vector<std::string>({"Invalid"});
  if (login_stack.empty() ||
      user_data_base.GetPrevilege(login_stack.top().first) < 7)
    return std::vector<std::string>({"Invalid"});
  if (count > log_data_base.TotalFinanceOperationCount())
    return std::vector<std::string>({"Invalid"});
  if (count == 0) return std::vector<std::string>({""});
  std::pair<double, double> ret = log_data_base.QueryFinance(count);
  std::string ans;
  unsigned long long income_tmp = ret.first * 100 + 0.5;
  unsigned long long outcome_tmp = ret.second * 100 + 0.5;
  ans =
      "+ " + std::to_string(income_tmp / 100) + '.' +
      std::to_string(income_tmp % 100 / 10) + std::to_string(income_tmp % 10) +
      " - " + std::to_string(outcome_tmp / 100) + '.' +
      std::to_string(outcome_tmp % 100 / 10) + std::to_string(outcome_tmp % 10);
  return std::vector<std::string>({ans});
}

std::vector<std::string> BookStoreEngineClass::ExecuteLog(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  static std::basic_regex<char> log_regex(
      R"(^ *log *$)", std::regex_constants::optimize);
  if(!std::regex_match(cmd, log_regex))
    return std::vector<std::string>({"Invalid"});
  return std::vector<std::string>();
}

std::vector<std::string> BookStoreEngineClass::ExecuteReport(
    const std::string &cmd,
    std::stack<std::pair<std::string, int>> &login_stack) {
  static std::basic_regex<char> report_regex(
      R"(^ *report( +finance| +employee) *$)", std::regex_constants::optimize);
  if (!std::regex_match(cmd, report_regex))
    return std::vector<std::string>({"Invalid"});
  return std::vector<std::string>();
}