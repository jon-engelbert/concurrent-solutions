// clang++ -std=c++14 reusable_barrier_class.cpp barrier.cpp -o reuseable_barrier.exe
// uses barrier class to set a barrier for arriving threads until 'allowed_max' threads arrive at the
// barrier.  Then, allowed_max threads are allowed through... but which ones?  In order to 'join' after running
// the critical section, we need to know which threads went through.  g_worker_indices collects the index of each
// thread that passed through the barrier.
// The cv is, in effect, a separate barrier to ensure that all threads that are allowed to pass the barrier have actually dones so 
// when g_set_is_done is true, then all active threads will be in g_worker_indices
// the main branch waits for all active threads to finish upon each thread's join method invocation.
// the Barrier's await method could be split into two... as in the little book of semaphores.
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>
#include "barrier.h"

std::mutex m;
std::mutex mutex_values;
std::condition_variable cv;
std::atomic_bool g_set_is_done(false);
std::vector<int> g_worker_indices;
// Barrier barrier1, barrier2;
int allowed_max = 3;


void worker_thread(int i, Barrier& barrier1)
{
	bool set_is_done = barrier1.await(m, i);

    {
        std::lock_guard<std::mutex> lk_guard(m);
        std::cout << "counts changing, is_arriving, number: " << i  << std::endl;
	    if (set_is_done) {
	        std::cout << "set is done with barrier, number: " << i  << std::endl;
	    }
    }
    {
    	std::lock_guard<std::mutex> lk_guard(m);
        g_worker_indices.push_back(i);
	    if (g_worker_indices.size() == allowed_max) {
	    	g_set_is_done = true;
	    	cv.notify_one();
	    }
	    // critical section here...
    }
}

int main()
{
    std::thread worker[50];
    int start_index = 0;

    Barrier barrier1(allowed_max);

    // these 6 threads are for testing only... in a real application, the threads would be created as needed, whenever needed.
    for (int i = 0; i < 6; i++)
        worker[i] = std::thread(worker_thread, i, std::ref(barrier1));

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initializing first thread batch "  << std::endl;
    }
    // this for statement could be replaced in the real world with an event loop that goes on and on...
    for (int i = 0; i < 2; i++)
    {
	    std::unique_lock<std::mutex> lk1(mutex_values);
        // std::lock_guard<std::mutex> lk_guard(m_wait);
        cv.wait(lk1, []{return (g_set_is_done.load());});
	    {
	    	std::lock_guard<std::mutex> lk_guard(m);
        	std::cout << "cv done waiting! "  << std::endl;
        }
    // join blocks until workers finishes execution
    	for (int j = 0; j < allowed_max; j++) {
    		// std::lock_guard<std::mutex> lk_guard(mutex_values);
	        worker[g_worker_indices[j]].join();
		}
    	g_worker_indices.clear();
    	g_set_is_done = false;
    	barrier1.reset();
    }
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), about to reset "  << std::endl;
	}



  //   for (int i = 6; i < 12; i++)
  //       worker[i] = std::thread(worker_thread, i, std::ref(barrier1));

  //   {
  //       std::lock_guard<std::mutex> lk(m);
  //       std::cout << "Back in main(), after initialing second thread batch." <<  std::endl;
  //   }

  //   // wait until worker dies finishes execution
  //   for (int i = 2; i < 4; i++) {
	 //    for (int j = i*allowed_max; j < (i+1) *allowed_max; j++) {
  //   		std::lock_guard<std::mutex> lk_guard(mutex_values);
	 //        worker[worker_index[j]].join();
		// }
	 //    {
	 //        std::lock_guard<std::mutex> lk(m);
	 //        std::cout << "Back in main(), about to reset "  << std::endl;
		//     barrier1.reset();
		// }
  //   }
    std::cout << "finished main() "  << std::endl;
}

