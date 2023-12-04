#ifndef BPT_DATABASE_HPP
#define BPT_DATABASE_HPP
#include <string>
#include <vector>

#include "drivearray.hpp"
#include "key2index.hpp"

template <class StorageType>
class DriveMultiMap_string {
 private:
  String2Index Indexer;
  DriveArray<StorageType, 2, 100> Storage;
  bool is_open = false;

 public:
  DriveMultiMap_string() = default;
  void OpenFile(const std::string __file_name) noexcept {
    Indexer.OpenFile(__file_name + ".idx");
    Storage.OpenFile(__file_name + ".dat");
    is_open = true;
  }
  std::vector<StorageType> Find(const std::string &key) noexcept {
    if (!is_open) return {};
    std::vector<StorageType> ret;
    std::vector<int> idxs = std::move(Indexer.Find(key));
    for (auto idx : idxs) {
      StorageType tmp;
      Storage.read(tmp, idx);
      ret.push_back(tmp);
    }
    return std::move(ret);
  }
  void Delete(const std::string &key, const StorageType &value) noexcept {
    if (!is_open) return;
    std::vector<int> idxs = std::move(Indexer.Find(key));
    for (auto idx : idxs) {
      StorageType tmp;
      Storage.read(tmp, idx);
      if (tmp == value) {
        Storage.Delete(idx);
        Indexer.Delete(key, idx);
        return;
      }
    }
  }
  void Insert(const std::string &key, StorageType &value) noexcept {
    if (!is_open) return;
    int idx = Storage.write(value);
    Indexer.Insert(key, idx);
  }
};
#endif  // BPT_DATABASE_HPP