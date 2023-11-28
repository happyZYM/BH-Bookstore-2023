#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <iostream>
#include <stack>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T, const int info_len = 2>
class MemoryRiver {
 private:
  /* your code here */
  fstream file;
  string file_name;
  const int sizeofT = sizeof(T);
  std::stack<int> free_mem;
  int total_mem = 0;

 public:
  MemoryRiver() = default;
  ~MemoryRiver() {
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekp(info_len * sizeof(int), std::ios::beg);
    // std::cerr << "pos1 " << info_len * sizeof(int) << std::endl;
    file.write(reinterpret_cast<char *>(&total_mem), sizeof(int));
    int tmp = free_mem.size();
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.seekp((info_len + 2) * sizeof(int) + total_mem * sizeofT,
               std::ios::beg);
    // std::cerr << "pos2 " << (info_len + 2) * sizeof(int) + total_mem * sizeofT
              // << std::endl;
    while (!free_mem.empty()) {
      tmp = free_mem.top();
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      free_mem.pop();
    }
    file.close();
  }
  bool operator=(const MemoryRiver &) = delete;

  MemoryRiver(const string &file_name) : file_name(file_name) {
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekg(info_len * sizeof(int), std::ios::beg);
    file.read(reinterpret_cast<char *>(&total_mem), sizeof(int));
    int free_mem_size = 0;
    file.read(reinterpret_cast<char *>(&free_mem_size), sizeof(int));
    file.seekg((info_len + 2) * sizeof(int) + total_mem * sizeofT,
               std::ios::beg);
    while (free_mem_size-- > 0) {
      int tmp;
      file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
      free_mem.push(tmp);
      fprintf(stderr,"get %d\n",tmp);
    }
    file.close();
  }

  void initialise(string FN = "") {
    if (FN != "") file_name = FN;
    file.open(file_name, std::ios::binary | std ::ios::out);
    file.seekp(0, std::ios::beg);
    int tmp = 0;
    for (int i = 0; i < info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  // 读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    if (n > info_len) return;
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekg((n - 1) * sizeof(int), std::ios::beg);
    // std::cerr << "pos " << (n - 1) * sizeof(int) << std::endl;
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  // 将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    if (n > info_len) return;
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekp((n - 1) * sizeof(int), std::ios::beg);
    file.write(reinterpret_cast<const char *>(&tmp), sizeof(int));
    file.close();
  }

  // 在文件合适位置写入类对象t，并返回写入的位置索引index
  // 位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  int write(T &t) {
    int index = -1;
    if (!free_mem.empty()) {
      index = free_mem.top();
      free_mem.pop();
    } else
      index = ++total_mem;
    update(t, index);
    return index;
  }

  // 用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, const int index) {
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekp((info_len + 2) * sizeof(int) + (index - 1) * sizeofT,
               std::ios::beg);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }

  // 读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index) {
    file.open(file_name, std::ios::binary | std ::ios::out | std::ios::in);
    file.seekg((info_len + 2) * sizeof(int) + (index - 1) * sizeofT,
               std::ios::beg);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }

  // 删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
  void Delete(int index) { free_mem.push(index); }
};

#endif  // BPT_MEMORYRIVER_HPP