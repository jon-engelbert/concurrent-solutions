// Copyright 2010 Google Inc. All Rights Reserved.

#ifndef GCL_BARRIER_
#define GCL_BARRIER_

#include <stddef.h>
#include <stdexcept>
#include <functional>

#include <thread>
#include <condition_variable>
#include <mutex>



// Allows a set of threads to wait until all threads have reached a
// common point.
class Barrier {
 public:
  // Creates a new barrier that will block until num_threads threads
  // are waiting on it. Throws invalid_argument if num_threads == 0.
  // explicit Barrier size_t num_threads) throw (std::invalid_argument);
  Barrier(const size_t num_threads) throw (std::invalid_argument);
  // Barrier(const Barrier&) {};
  // ~Barrier() {};

  // Creates a new barrier that will block until num_threads threads
  // are waiting on it. When the barrier is released, function will
  // be invoked. Throws invalid_argument if num_threads == 0.
  // Barrier(size_t num_threads, std::function<void()> function)
  //     throw (std::invalid_argument);

  // Blocks until num_threads have call await(). Invokes the function
  // specified in the constructor before releasing any thread. Returns
  // true to one of the callers, and false to the others, allowing one
  // caller to delete the barrier.
  //
  // Memory ordering: For threads X and Y that call await(), the call
  // to await() in X happens before the return from await() in Y.
  //
  // A logic_error will be thrown if more than num_threads call await().
  bool await(std::mutex& m, int thread_index) throw (std::logic_error);

  // reset num_to_block_left and num_to_exit
  void reset();

 private:
  std::mutex lock_;
  std::atomic_bool is_closed_;
  std::atomic_int num_threads_orig_;
  std::atomic_int num_to_block_left_;
  std::atomic_int num_to_exit_;
  std::condition_variable cv_;
  std::function<void()> function_;
};

#endif // GCL_BARRIER_