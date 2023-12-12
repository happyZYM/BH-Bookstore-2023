#include <iostream>
#include <string>

#include "bs-utility.h"
#include "builtin-cli.h"
#include "clipp/clipp.h"
int main(int argc, char **argv) {
  // test();
  // return 0;
  bool is_server = false;
  std::string config_dir = "";
  bool custom_config_dir = false;
  auto cli =
      (clipp::option("-s", "--server").set(is_server).doc("run as server"),
       clipp::option("-c", "--config")
               .set(custom_config_dir)
               .doc("use config directory") &
           clipp::value("config directory", config_dir));
  if (!clipp::parse(argc, argv, cli)) {
    std::cout << clipp::make_man_page(cli, argv[0]);
    return 0;
  }
  if (!custom_config_dir) config_dir = "./";
  try {
    BookStoreMain(is_server, config_dir);
  } catch (const FatalError &e) {
    std::cerr << "\e[7m\e[31m[Fatal Error] " << e.GetCode() << " : " << e.what()
              << "\e[0m" << std::endl;
    return e.GetCode();
  } catch (const std::exception &e) {
    std::cerr << "\e[7m\e[31m[other std::exception] " << e.what() << "\e[0m"
              << std::endl;
    return 255;
  } catch (...) {
    std::cerr << "\e[7m\e[31m[Unknown Exception]\e[0m" << std::endl;
    return 255;
  }
  return 0;
}