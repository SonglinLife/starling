#include <liburing.h>
#include "aligned_file_reader.h"

// wirte a std::vector aligned allocator for char
template<typename T>
class AlignedAllocator {
 public:
  using value_type = T;
  AlignedAllocator() = default;

  T* allocate(std::size_t n);
  void deallocate(T* p, size_t n);
};

class UringReader {
 public:
  UringReader(int fd, int MAXIONUM = 1024);
  UringReader(std::string filepath, int MAXIONUM = 1024);
  int submit_IOs(std::vector<AlignedRead>& reqs);
  int waitCompl();
  std::pair<uint64_t, void*> waitOne();
  ~UringReader(); 
 private:
  static constexpr size_t PAGESIZE = 4096l;
  // a aligned allocator vector of char
  std::vector<char, AlignedAllocator<char>> bufs;
  std::vector<std::pair<uint64_t, void*>> idwithbuf;
  std::vector<iovec>                        iov;
  int                                       sz = 1024;
  int                                       bufferIndex = 0;
  int                                       inflight = 0;
  struct io_uring                           ring;
};
