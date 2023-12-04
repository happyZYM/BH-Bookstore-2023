#ifndef BPT_KEY2INDEX_HPP
#define BPT_KEY2INDEX_HPP
#include <cassert>
#include <functional>
#include <string>
#include <vector>

#include "drivearray.hpp"
class String2Index {
 private:
  static const int kBucketSize = 262142;
  static const int kPageSize = 4096;

  struct Node {
    char str[66];
    int val;
    Node() = default;
    Node(const std::string &_str, int _val) : val(_val) {
      assert(_str.length() <= 64);
      strcpy(str, _str.c_str());
    }
  };
  static const int kNodesPerBlock =
      (kPageSize - 2 * sizeof(int)) / sizeof(Node);

  struct Block {
    int tot, nxt_idx;
    Node data[kNodesPerBlock];
    char padding[kPageSize - 2 * sizeof(int) - sizeof(Node) * (kNodesPerBlock)];
    Block() : tot(0), nxt_idx(0) {}
    Block(int _tot, int _nxt_idx) : tot(_tot), nxt_idx(_nxt_idx) {}
  };
  static_assert(kNodesPerBlock >= 1, "kNodesPerBlock error");
  static_assert(sizeof(Block) == kPageSize, "Block Size error");

  DriveArray<Block, kBucketSize, 10> mem;
  int *hash_table = nullptr;
  std::string file_name;

  inline size_t Hash(std::string str) noexcept {
    const static std::string salt1 = "mL;]-=eT";
    const static std::string salt2 = "9B<mF_me";
    str = salt1 + str + salt2;
    size_t ret = 0;
    for (int i = 0; i < str.length(); ++i) ret = ret * 131 + str[i];
    return ret;
  }

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
    for (int i = 0; i < kBucketSize; i++) {
      hash_table[i] = *((int *)(mem.RawData()) + i);
      if (i % 4096 == 0) mem.ForceRefresh();
    }
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
      for (int i = 0; i < kBucketSize; i++) {
        *((int *)(mem.RawData()) + i) = hash_table[i];
        if (i % 4096 == 0) mem.ForceRefresh();
      }
      delete[] hash_table;
    }
  }
  void Insert(const std::string &str, int val) noexcept {
    size_t hash_val = Hash(str);
    int idx = hash_table[hash_val % kBucketSize];
    Block *blk_ptr = new Block;
    if (idx == 0) {
      Block __Init_Block;
      idx = mem.write(__Init_Block);
      hash_table[hash_val % kBucketSize] = idx;
      assert(idx >= 1);
    }
    mem.read(*blk_ptr, idx);
    if (blk_ptr->tot == kNodesPerBlock) {
      Block __New_Head_Block(0, idx);
      idx = mem.write(__New_Head_Block);
      hash_table[hash_val % kBucketSize] = idx;
      mem.read(*blk_ptr, idx);
    }
    blk_ptr->data[blk_ptr->tot++] = Node(str, val);
    mem.update(*blk_ptr, idx);
    delete blk_ptr;
  }
  void Delete(const std::string &str, int val) noexcept {
    size_t hash_val = Hash(str);
    int idx = hash_table[hash_val % kBucketSize];
    Block *blk_ptr = new Block;
    while (idx != 0) {
      mem.read(*blk_ptr, idx);
      for (int i = 0; i < blk_ptr->tot; ++i) {
        if (blk_ptr->data[i].str == str && blk_ptr->data[i].val == val) {
          int headidx = hash_table[hash_val % kBucketSize];
          if (headidx == idx) {
            blk_ptr->data[i] = blk_ptr->data[--blk_ptr->tot];
            mem.update(*blk_ptr, idx);
          } else {
            Block *head_blk_ptr = new Block;
            mem.read(*head_blk_ptr, headidx);
            blk_ptr->data[i] = head_blk_ptr->data[--head_blk_ptr->tot];
            if (head_blk_ptr->tot == 0) {
              hash_table[hash_val % kBucketSize] = head_blk_ptr->nxt_idx;
              mem.Delete(headidx);
            } else
              mem.update(*head_blk_ptr, headidx);
            mem.update(*blk_ptr, idx);
            delete head_blk_ptr;
          }
          delete blk_ptr;
          return;
        }
      }
      idx = blk_ptr->nxt_idx;
    }
    delete blk_ptr;
  }
  std::vector<int> Find(const std::string &str) noexcept {
    std::vector<int> ret;
    size_t hash_val = Hash(str);
    int idx = hash_table[hash_val % kBucketSize];
    Block *blk_ptr = new Block;
    while (idx != 0) {
      mem.read(*blk_ptr, idx);
      for (int i = 0; i < blk_ptr->tot; ++i) {
        if (blk_ptr->data[i].str == str) {
          ret.push_back(blk_ptr->data[i].val);
        }
      }
      idx = blk_ptr->nxt_idx;
    }
    delete blk_ptr;
    return std::move(ret);
  }
};
#endif  // BPT_KEY2INDEX_HPP