#ifndef BPT_DriveArray_HPP
#define BPT_DriveArray_HPP

#include <algorithm>
#include <cstring>
#include <fstream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
template <class T, const int info_len = 2, const int kBufSize = 100>
class DriveArray {
 private:
  static const int kPageSize = 4096;
  static const int kDataBiginOffset =
      ((info_len + 2) * sizeof(int) + kPageSize - 1) / kPageSize * kPageSize;
  static const int sizeofT = sizeof(T);
  struct DataType {
    int next_vacant_data_index;
    T val;
  };
  static const int kBlockSize =
      (sizeof(DataType) + kPageSize - 1) / kPageSize * kPageSize;
  static const int kDataPerBlock = kBlockSize / sizeof(DataType);
  struct BlockType {
    DataType data[kDataPerBlock];
  };
  char rest[kBlockSize - sizeof(BlockType)];
  static_assert(kBlockSize % kPageSize == 0, "kBlockSize % kPageSize != 0");
  std::string file_name;
  int total_block_number = 0, first_vacant_data_index = 0;
  /**
   * DataIndex=(BlockIndex-1)*kDataPerBlock+InnnerIndex
   */
  std::unordered_map<int, BlockType *> cache;
  std::queue<int> vis_que;
  void LoadCache(int block_index) {
    BlockType *tmp = new BlockType;
    fs.seekg(kDataBiginOffset + (block_index - 1) * kBlockSize, std::ios::beg);
    fs.read(reinterpret_cast<char *>(tmp), sizeof(BlockType));
    cache[block_index] = tmp;
    vis_que.push(block_index);
  }
  void ReleaseOldestCache() {
    int block_index = vis_que.front();
    vis_que.pop();
    fs.seekp(kDataBiginOffset + (block_index - 1) * kBlockSize, std::ios::beg);
    fs.write(reinterpret_cast<char *>(cache[block_index]), sizeof(BlockType));
    delete cache[block_index];
    cache.erase(block_index);
  }
  BlockType *OrderBlock(int block_index) {
    if (cache.find(block_index) != cache.end()) return cache[block_index];
    if (cache.size() == kBufSize) ReleaseOldestCache();
    LoadCache(block_index);
    return cache[block_index];
  }
  int AppEndBlock() {
    fs.seekp(0, std::ios::end);
    BlockType tmp;
    fs.write(reinterpret_cast<char *>(&tmp), sizeof(BlockType));
    fs.write(rest, kBlockSize - sizeof(BlockType));
    ++total_block_number;
    return total_block_number;
  }

 public:
  std::fstream fs;
  DriveArray() = default;
  DriveArray(const DriveArray &) = delete;
  DriveArray &operator=(const DriveArray &) = delete;
  DriveArray(const std::string &file_name) : file_name(file_name) {
    OpenFile(file_name);
  }

  inline bool IsOpen() const noexcept { return fs.is_open(); }
  ~DriveArray() { CloseFile(); }
  void CloseFile() {
    if (!fs.is_open()) return;
    while (cache.size() > 0) ReleaseOldestCache();
    fs.seekp(sizeof(int) * info_len, std::ios::beg);
    fs.write(reinterpret_cast<char *>(&first_vacant_data_index), sizeof(int));
    fs.write(reinterpret_cast<char *>(&total_block_number), sizeof(int));
    fs.close();
    file_name = "";
    first_vacant_data_index = 0;
    total_block_number = 0;
  }
  void OpenFile(const std::string &__file_name) {
    if (fs.is_open()) CloseFile();
    file_name = __file_name;
    fs.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    if (!fs.is_open()) {
      fs.open(file_name, std::ios::out | std::ios::binary);
      fs.seekp(0, std::ios::beg);
      int tmp = 0;
      total_block_number = 0;
      first_vacant_data_index = 0;
      for (int i = 0; i < kDataBiginOffset / sizeof(int); ++i) {
        fs.write(reinterpret_cast<char *>(&tmp), sizeof(int));
      }
      fs.close();
      fs.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
    fs.seekg(sizeof(int) * info_len, std::ios::beg);
    fs.read(reinterpret_cast<char *>(&first_vacant_data_index), sizeof(int));
    fs.read(reinterpret_cast<char *>(&total_block_number), sizeof(int));
  }
  void initialise(std::string FN = "") {
    if (fs.is_open()) {
      std::string name_bak = file_name;
      CloseFile();
      file_name = name_bak;
    }
    if (FN != "") file_name = FN;
    if (file_name == "") return;
    fs.open(file_name, std::ios::out | std::ios::binary);
    fs.seekp(0, std::ios::beg);
    int tmp = 0;
    total_block_number = 0;
    first_vacant_data_index = 0;
    for (int i = 0; i < kDataBiginOffset / sizeof(int); ++i) {
      fs.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    }
    fs.close();
    fs.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
  }

  void get_info(int &tmp, int n) noexcept {
    if (n > info_len) return;
    fs.seekg((n - 1) * sizeof(int), std::ios::beg);
    fs.read(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  void write_info(int tmp, int n) noexcept {
    if (n > info_len) return;
    fs.seekp((n - 1) * sizeof(int), std::ios::beg);
    fs.write(reinterpret_cast<char *>(&tmp), sizeof(int));
  }

  void LoadInfoTo(int *dest) {
    fs.seekg(0, std::ios::beg);
    fs.read(reinterpret_cast<char *>(dest), sizeof(int) * info_len);
  }

  void WriteInfoFrom(int *src) {
    fs.seekp(0, std::ios::beg);
    fs.write(reinterpret_cast<char *>(src), sizeof(int) * info_len);
  }

  int write(T &t) noexcept {
    if (first_vacant_data_index == 0) {
      int new_block_index = AppEndBlock();
      int index = (new_block_index - 1) * kDataPerBlock + 1;
      BlockType *blk_ptr = OrderBlock(new_block_index);
      if (kDataPerBlock > 1)
        first_vacant_data_index = index + 1;
      else
        first_vacant_data_index = 0;
      for (int i = 1; i < kDataPerBlock - 1; i++)
        blk_ptr->data[i].next_vacant_data_index = index + i + 1;
      blk_ptr->data[kDataPerBlock - 1].next_vacant_data_index = 0;
      blk_ptr->data[0].next_vacant_data_index = 0;
      blk_ptr->data[0].val = t;
      return index;
    } else {
      int block_index = (first_vacant_data_index - 1) / kDataPerBlock + 1;
      int inner_index = (first_vacant_data_index - 1) % kDataPerBlock + 1;
      BlockType *blk_ptr = OrderBlock(block_index);
      int index = first_vacant_data_index;
      first_vacant_data_index =
          blk_ptr->data[inner_index - 1].next_vacant_data_index;
      blk_ptr->data[inner_index - 1].next_vacant_data_index = 0;
      blk_ptr->data[inner_index - 1].val = t;
      return index;
    }
  }

  void update(T &t, const int index) noexcept {
    int block_index = (index - 1) / kDataPerBlock + 1;
    int inner_index = (index - 1) % kDataPerBlock + 1;
    BlockType *blk_ptr = OrderBlock(block_index);
    blk_ptr->data[inner_index - 1].val = t;
  }

  void read(T &t, const int index) noexcept {
    int block_index = (index - 1) / kDataPerBlock + 1;
    int inner_index = (index - 1) % kDataPerBlock + 1;
    BlockType *blk_ptr = OrderBlock(block_index);
    t = blk_ptr->data[inner_index - 1].val;
  }

  void Delete(int index) noexcept {
    int block_index = (index - 1) / kDataPerBlock + 1;
    int inner_index = (index - 1) % kDataPerBlock + 1;
    BlockType *blk_ptr = OrderBlock(block_index);
    blk_ptr->data[inner_index - 1].next_vacant_data_index =
        first_vacant_data_index;
    first_vacant_data_index = index;
  }
};

#endif  // BPT_DriveArray_HPP