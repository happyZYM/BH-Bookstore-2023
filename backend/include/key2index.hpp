#ifndef PROTECTOR_KEY2INDEX_HPP
#define PROTECTOR_KEY2INDEX_HPP
#include <cassert>
#include <functional>
#include <string>
#include <vector>

#include "drivearray.hpp"
class String2Index {
 private:
  static const int kBucketSize = 262142;
  static const int kPageSize = 4096;
  constexpr static char salt1[10] = "mL;]-=eT";
  constexpr static char salt2[10] = "9B<mF_me";
  constexpr static char sub_salt1[10] = "23333";
  constexpr static char sub_salt2[10] = "...";
  constexpr static char inner_salt[17] = "si9aW@zl#2$3%4^!";

  typedef unsigned long long hash_t;
  static_assert(sizeof(hash_t) == 8);
  static inline hash_t Hash(std::string str) noexcept {
    /* Reference: http://xorshift.di.unimi.it/splitmix64.c */
    str = salt1 + str + salt2;
    hash_t ret = 0;
    int i = 0;
    for (; i + 8 <= str.length(); i += 8) {
      ret ^= *reinterpret_cast<const hash_t *>(str.c_str() + i);
      ret ^= *reinterpret_cast<const hash_t *>(inner_salt + (i & 15));
      ret += 0x9e3779b97f4a7c15;
      ret = (ret ^ (ret >> 30)) * 0xbf58476d1ce4e5b9;
      ret = (ret ^ (ret >> 27)) * 0x94d049bb133111eb;
      ret ^= ret >> 31;
    }
    for (; i < str.length(); ++i) {
      ret ^= str[i];
      ret ^= inner_salt[i & 15];
      ret += 0x9e3779b97f4a7c15;
      ret = (ret ^ (ret >> 30)) * 0xbf58476d1ce4e5b9;
      ret = (ret ^ (ret >> 27)) * 0x94d049bb133111eb;
      ret ^= ret >> 31;
    }
    return ret;
  }

  struct Node {
    hash_t main_hash, sub_hash;
    int val, nxt_idx = 0;
    Node() = default;
    Node(std::string str, int _val) {
      main_hash = Hash(str);
      sub_hash = Hash(sub_salt1 + str + sub_salt2);
      val = _val;
    }
  };

  DriveArray<Node, kBucketSize, 100> mem;
  int *hash_table = nullptr;
  std::string file_name;

 public:
  String2Index() = default;
  void OpenFile(const std::string __file_name) noexcept {
    file_name = __file_name;
    if (file_name == "") return;
    if (mem.IsOpen())
      for (int i = 0; i < kBucketSize; ++i)
        mem.write_info(hash_table[i], i + 1);
    mem.OpenFile(file_name);
    delete[] hash_table;
    hash_table = new int[kBucketSize];
    // std::memmove(hash_table, mem.RawData(), sizeof(int) * kBucketSize);
    // mem.ForceRefresh();
    mem.LoadInfoTo(hash_table);
  }
  String2Index(const std::string __file_name) : file_name(__file_name) {
    OpenFile(file_name);
  }
  void initialise(std::string FN = "") noexcept {
    if (FN != "") file_name = FN;
    if (mem.IsOpen())
      for (int i = 0; i < kBucketSize; ++i)
        mem.write_info(hash_table[i], i + 1);
    delete[] hash_table;
    mem.initialise(file_name);
    hash_table = new int[kBucketSize]();
  }
  ~String2Index() {
    if (hash_table != nullptr) {
      mem.WriteInfoFrom(hash_table);
      delete[] hash_table;
    }
  }
  void Insert(const std::string &str, int val) noexcept {
    hash_t hash_val = Hash(str);
    int idx = hash_table[hash_val % kBucketSize];
    Node nd(str, val);
    if (idx == 0) {
      idx = mem.write(nd);
      hash_table[hash_val % kBucketSize] = idx;
    } else {
      nd.nxt_idx = idx;
      idx = mem.write(nd);
      hash_table[hash_val % kBucketSize] = idx;
    }
  }
  void Delete(const std::string &str, int val) noexcept {
    hash_t str_main_hash = Hash(str),
           str_sub_hash = Hash(sub_salt1 + str + sub_salt2);
    int idx = hash_table[str_main_hash % kBucketSize];
    Node nd, last_nd;
    int last_idx = -1;
    while (idx != 0) {
      mem.read(nd, idx);
      if (nd.main_hash == str_main_hash && nd.sub_hash == str_sub_hash &&
          nd.val == val) {
        if (last_idx == -1) {
          hash_table[str_main_hash % kBucketSize] = nd.nxt_idx;
          mem.Delete(idx);
        } else {
          last_nd.nxt_idx = nd.nxt_idx;
          mem.update(last_nd, last_idx);
          mem.Delete(idx);
        }
        return;
      }
      last_idx = idx;
      last_nd = nd;
      idx = nd.nxt_idx;
    }
  }
  std::vector<int> Find(const std::string &str) noexcept {
    std::vector<int> ret;
    hash_t str_main_hash = Hash(str),
           str_sub_hash = Hash(sub_salt1 + str + sub_salt2);
    int idx = hash_table[str_main_hash % kBucketSize];
    Node nd;
    while (idx != 0) {
      mem.read(nd, idx);
      if (nd.main_hash == str_main_hash && nd.sub_hash == str_sub_hash) {
        ret.push_back(nd.val);
      }
      idx = nd.nxt_idx;
    }
    return std::move(ret);
  }
};
#endif  // PROTECTOR_KEY2INDEX_HPP