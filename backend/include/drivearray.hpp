#ifndef BPT_DriveArray_HPP
#define BPT_DriveArray_HPP

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <stack>
#include <string>

template <class T, const int info_len = 2, const int kRefreshThreshold = 100>
class DriveArray {
 private:
  static const int kPageSize = 4096;
  int file_descriptor = -1;
  size_t file_length = 0;
  void *virtual_mem;
  std::string file_name;
  const int sizeofT = sizeof(T);
  const int raw_data_begin =
      ((info_len + 2) * sizeof(int) + kPageSize - 1) / kPageSize * kPageSize;
  std::stack<int> free_mem;
  int total_mem = 0;
  unsigned int forced_refresh = 0;
  void reallocate(bool include_resync = false) noexcept {
    size_t length_needed =
        raw_data_begin +
        (sizeofT * total_mem + kPageSize - 1) / kPageSize * kPageSize;
    if (include_resync)
      length_needed += (free_mem.size() * sizeof(int) + kPageSize - 1) /
                       kPageSize * kPageSize;
    if (file_length >= length_needed) return;
    munmap(virtual_mem, file_length);
    file_length = std::max(file_length * 2, length_needed);
    ftruncate(file_descriptor, file_length);
    virtual_mem = mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_SHARED,
                       file_descriptor, 0);
  }
  void ForceRefresh() noexcept {
    munmap(virtual_mem, file_length);
    virtual_mem = mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_SHARED,
                       file_descriptor, 0);
  }

 public:
  DriveArray() = default;
  inline bool IsOpen() const noexcept { return file_descriptor >= 0; }
  ~DriveArray() {
    reallocate(true);
    int stk_data_begin =
        raw_data_begin +
        (sizeofT * total_mem + kPageSize - 1) / kPageSize * kPageSize;
    *((int *)(virtual_mem) + info_len) = total_mem;
    *((int *)(virtual_mem) + info_len + 1) = (int)free_mem.size();
    int *p = (int *)(virtual_mem + stk_data_begin);
    while (!free_mem.empty()) {
      *(p++) = free_mem.top();
      free_mem.pop();
    }
    munmap(virtual_mem, file_length);
    close(file_descriptor);
    file_descriptor = -1;
  }
  bool operator=(const DriveArray &) = delete;

  void OpenFile(const std::string &file_name) {
    if (file_name == "") return;
    if (file_descriptor >= 0) {
      munmap(virtual_mem, file_length);
      close(file_descriptor);
      file_descriptor = -1;
    }
    file_descriptor =
        open(file_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    struct stat file_state;
    fstat(file_descriptor, &file_state);
    file_length = file_state.st_size;
    if (file_length == 0) {
      file_length = 1024 * 1024;
      file_length =
          std::max(file_length, ((info_len + 2) * sizeof(int) + kPageSize - 1) /
                                    kPageSize * kPageSize);
      ftruncate(file_descriptor, file_length);
    }
    virtual_mem = mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_SHARED,
                       file_descriptor, 0);
    total_mem = *((int *)(virtual_mem) + info_len);
    int free_mem_cnt = *((int *)(virtual_mem) + info_len + 1);
    int stk_data_begin =
        raw_data_begin +
        (sizeofT * total_mem + kPageSize - 1) / kPageSize * kPageSize;
    int *p = (int *)(virtual_mem + stk_data_begin);
    for (int i = 0; i < free_mem_cnt; i++) {
      free_mem.push(*(p++));
    }
  }
  DriveArray(const std::string &file_name) : file_name(file_name) {
    OpenFile(file_name);
  }

  void initialise(std::string FN = "") {
    if (FN != "") file_name = FN;
    if (file_descriptor >= 0) {
      munmap(virtual_mem, file_length);
      close(file_descriptor);
      file_descriptor = -1;
    }
    file_descriptor =
        open(file_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    file_length = 1024 * 1024;
    file_length =
        std::max(file_length, ((info_len + 2) * sizeof(int) + kPageSize - 1) /
                                  kPageSize * kPageSize);
    ftruncate(file_descriptor, file_length);
    virtual_mem = mmap(nullptr, file_length, PROT_READ | PROT_WRITE, MAP_SHARED,
                       file_descriptor, 0);
    for (int i = 0; i < info_len; i++) *((int *)(virtual_mem) + i) = 0;
  }

  void get_info(int &tmp, int n) noexcept {
    if (n > info_len) return;
    tmp = *((int *)(virtual_mem) + n - 1);
    if (++forced_refresh >= kRefreshThreshold) {
      forced_refresh = 0;
      ForceRefresh();
    }
  }

  void write_info(int tmp, int n) noexcept {
    if (n > info_len) return;
    *((int *)(virtual_mem) + n - 1) = tmp;
    if (++forced_refresh >= kRefreshThreshold) {
      forced_refresh = 0;
      ForceRefresh();
    }
  }

  int write(T &t) noexcept {
    int index = -1;
    if (!free_mem.empty()) {
      index = free_mem.top();
      free_mem.pop();
    } else
      index = ++total_mem;
    update(t, index);
    return index;
  }

  void update(T &t, const int index) noexcept {
    reallocate();
    void *data_begin = virtual_mem + raw_data_begin + sizeofT * (index - 1);
    std::memmove(data_begin, &t, sizeofT);
    // madvise(data_begin, sizeofT, MADV_FREE);
    if (++forced_refresh >= kRefreshThreshold) {
      forced_refresh = 0;
      ForceRefresh();
    }
  }

  void read(T &t, const int index) noexcept {
    reallocate();
    void *data_begin = virtual_mem + raw_data_begin + sizeofT * (index - 1);
    std::memmove(&t, data_begin, sizeofT);
    // madvise(data_begin, sizeofT, MADV_FREE);
    if (++forced_refresh >= kRefreshThreshold) {
      forced_refresh = 0;
      ForceRefresh();
    }
  }

  void Delete(int index) noexcept { free_mem.push(index); }
};

#endif  // BPT_DriveArray_HPP