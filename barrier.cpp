// based on code by the same name in the google concurrency library

#include "barrier.h"
#include <iostream>

Barrier::Barrier(const size_t num_threads) throw (std::invalid_argument)
    : num_to_block_left_(num_threads),
      num_to_exit_(num_threads),
      num_threads_orig_(num_threads),
      is_closed_(true),
      function_(NULL) {
  if (num_threads == 0) {
    throw std::invalid_argument("num_threads must be > 0");
  }
}

// Barrier::Barrier(size_t num_threads, std::function<void()> function)
//     throw (std::invalid_argument)
//     : num_to_block_left_(num_threads),
//       num_to_exit_(num_threads),
//       num_threads_orig_(num_threads),
//       function_(function) {
//   if (num_threads == 0) {
//     throw std::invalid_argument("num_threads must be > 0");
//   }
// }

bool Barrier::await(std::mutex& m, int thread_index)  throw (std::logic_error) {
  bool is_complete = false;
  {
    std::lock_guard<std::mutex> lk_guard(m);
    std::cout << "waiting in barrier: " << thread_index << std::endl;
  }
  std::unique_lock<std::mutex> ul(lock_);
  num_to_block_left_--;
  cv_.wait(ul, [this]{return (num_to_block_left_ == 0 || !is_closed_) && num_to_exit_ > 0;});

  if (function_ != NULL) {
    function_();
  }

  is_closed_ = false;
  cv_.notify_all();
  num_to_block_left_++;
  {
    std::lock_guard<std::mutex> lk_guard(m);
    std::cout << "about to exit: " << num_to_exit_ << std::endl;
  }
  num_to_exit_--;
  // don't automatically reset the barrier when it's complete!
  is_complete = (num_to_exit_ == 0);
  // if (is_complete) {
  //   is_closed_ = true;
  //   num_to_exit_.store(num_threads_orig_);
  // }
  return (is_complete);
}

// reset the barrier 'lock', but retain the count of waiting threads (num_to_block_left)
void Barrier::reset() {
  std::lock_guard<std::mutex> lk_guard(lock_);
  // num_to_block_left_.store(num_threads_orig_);
  is_closed_ = true;
  num_to_exit_.store(num_threads_orig_);
  cv_.notify_all();
}

