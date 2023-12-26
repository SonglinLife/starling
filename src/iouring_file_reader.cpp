#include "iouring_file_reader.h"
#include <liburing/io_uring.h>
#include <cstdint>

template<typename T> 
T* AlignedAllocator<T>::allocate(std::size_t n){
    // new a aligned memory
    return static_cast<T*>(operator new(n, std::align_val_t(4096)));
}

template<typename T>
void AlignedAllocator<T>::deallocate(T* p, size_t n){
    // delete a aligned memory
    operator delete(p);
}

UringReader::UringReader(int fd, int MAXIONUM) {
  int ret = io_uring_queue_init(MAXIONUM, &ring, IORING_SETUP_SQPOLL);
  if (ret < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    exit(1);
  }
  ret = io_uring_register_files(&ring, &fd, 1);
  if (ret) {
    std::cout << "io_uring_register_files error" << std::endl;
    exit(1);
  }
  bufs.resize(PAGESIZE * MAXIONUM);
  iov.resize(MAXIONUM);
  idwithbuf.resize(MAXIONUM);
  for (int i = 0; i < MAXIONUM; ++i) {
    // register buffer
    iov[i].iov_base = bufs.data() + PAGESIZE * i;
    iov[i].iov_len = PAGESIZE;
  }
  ret = io_uring_register_buffers(&ring, iov.data(), MAXIONUM);
  if (ret) {
    std::cout << "io_uring_register_buffers error" << std::endl;
    exit(1);
  }
}

UringReader::UringReader(std::string filepath, int MAXIONUM) {
  int fd = open(filepath.c_str(), O_RDONLY | O_DIRECT);
  if (fd < 0) {
    std::cout << "open file error" << std::endl;
    exit(1);
  }
  int ret = io_uring_queue_init(MAXIONUM, &ring, IORING_SETUP_SQPOLL);
  if (ret < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    exit(1);
  }
  ret = io_uring_register_files(&ring, &fd, 1);
  if (ret) {
    std::cout << "io_uring_register_files error" << std::endl;
    exit(1);
  }
  bufs.resize(PAGESIZE * MAXIONUM);
  iov.resize(MAXIONUM);
  idwithbuf.resize(MAXIONUM);
  for (int i = 0; i < MAXIONUM; ++i) {
    // register buffer
    iov[i].iov_base = bufs.data() + PAGESIZE * i;
    iov[i].iov_len = PAGESIZE;
  }
  ret = io_uring_register_buffers(&ring, iov.data(), MAXIONUM);
  if (ret) {
    std::cout << "io_uring_register_buffers error" << std::endl;
    exit(1);
  }
}
int UringReader::submit_IOs(std::vector<AlignedRead>& reqs) {
  for (auto& req : reqs) {
    // get sqe
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
      std::cout << "io_uring_get_sqe error" << std::endl;
      exit(1);
    }
    // set sqe
    io_uring_prep_read_fixed(sqe, 0, iov[bufferIndex].iov_base, req.len,
                             req.offset, bufferIndex);

    io_uring_sqe_set_flags(sqe, IOSQE_FIXED_FILE);

    req.buf = iov[bufferIndex].iov_base;
    idwithbuf[bufferIndex] = std::pair{req.id, iov[bufferIndex].iov_base}; 

    io_uring_sqe_set_data(sqe, &(idwithbuf[bufferIndex]));

    bufferIndex = (bufferIndex + 1) % sz;
  }
  // submit IOs
  int ret = io_uring_submit(&ring);
  if (ret != (int) reqs.size()) {
    std::cout << "io_uring_submit error the req size is "<<reqs.size()<<" the ret is "<< ret << std::endl;
    exit(1);
  }
  inflight += reqs.size();
  return ret;
}
int UringReader::waitCompl() {
  struct io_uring_cqe* cqe;
  // wait inflight
  int ret = io_uring_wait_cqe_nr(&ring, &cqe, inflight);
  if (ret < 0) {
    std::cout << "io_uring_wait_cqe_nr error" << std::endl;
    exit(1);
  }
  // advance inflight
  io_uring_cq_advance(&ring, inflight);

  inflight = 0;

  return 0;
}
std::pair<uint64_t, void*> UringReader::waitOne(){
  struct io_uring_cqe* cqe;
  // wait one
  int ret = io_uring_wait_cqe(&ring, &cqe);
  if (ret < 0) {
    std::cout << "io_uring_wait_cqe error" << std::endl;
    exit(1);
  }
  std::pair<uint64_t, void*>* idbuf = (std::pair<uint64_t, void*> *)io_uring_cqe_get_data(cqe);
  // advance one
  io_uring_cq_advance(&ring, 1);
  inflight -= 1;
  return *idbuf;
}
UringReader::~UringReader() {
  io_uring_unregister_buffers(&ring);
  io_uring_unregister_files(&ring);
  io_uring_queue_exit(&ring);
  bufs.clear();
  iov.clear();
}

