// http://en.cppreference.com/w/cpp/thread/condition_variable
// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, barrierMutex1, barrierMutex2, m2, m3, m4;
std::condition_variable cv1, cv2;
std::string data;
std::atomic_bool arrived_all, processed_all;
std::atomic_int arrived_count, processed_count;
int allowed_max = 3;


class Barrier {
public:
    int wait_count;
    int const target_wait_count;
    std::mutex mtx;
    std::condition_variable cond_var;

    Barrier(int threads_to_wait_for)
     : wait_count(0), target_wait_count(threads_to_wait_for) {}

    void wait() {
        std::unique_lock<std::mutex> lk(mtx);
        ++wait_count;
        if(wait_count != target_wait_count) {
            // not all threads have arrived yet; go to sleep until they do
            cond_var.wait(lk, 
                [this]() { return wait_count == target_wait_count; });
        } else {
            // we are the last thread to arrive; wake the others and go on
            cond_var.notify_all();
        }
        // note that if you want to reuse the barrier, you will have to
        // reset wait_count to 0 now before calling wait again
        // if you do this, be aware that the reset must be synchronized with
        // threads that are still stuck in the wait
    }
};


void worker_thread(int i, class Barrier& barrier1, class Barrier& barrier2)
{
    {
        // std::lock_guard<std::mutex> lk(m);
        std::lock_guard<std::mutex> lk_guard(m);
        std::cout << "Worker thread "  << i << " has arrived." << std::endl;
    }

    barrier1.wait();

    {
        std::lock_guard<std::mutex> lk_guard(m);
        std::cout << "counts changing2, processed_all: " << processed_all << ", count > max:" << (arrived_count < allowed_max) << std::endl;
        arrived_count++;
        processed_count--;
        std::cout << "counts changing3, processed: " << processed_count << ", arrived:" << arrived_count<< std::endl;
    }

    {
        std::lock_guard<std::mutex> lk_guard(m);
        if (arrived_count.load() >= allowed_max) {
            std::cout << "Enough arrived, about to open floodgate #1" << std::endl;
            arrived_all = true;
            processed_all = false;
            // for (int i = 0; i < allowed_max; i++) {
            //     cv1.notify_one();
            // }
        }
        std::cout << "about to reach 2nd barrier:" << arrived_count<< std::endl;

    }
    // std::unique_lock<std::mutex> lk1(barrier1);
    barrier2.wait();
    // cv1.wait(lk1, []{return (processed_count < allowed_max) && arrived_all;});
    {
        std::lock_guard<std::mutex> lk_guard(m);
        processed_count++;
        arrived_count--;
    }

    // critical section would go here... then increment processed_count

    // report after critical section
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread "  << i << " data processing completed" << std::endl;
        if (processed_count == allowed_max) {
            std::cout << "Group finished, about to open floodgate #2" << std::endl;
            processed_all = true;
            arrived_all = false;
            // for (int i = 0; i < allowed_max; i++) {
            //     cv2.notify_one();
            // }
        }
    }
}

int main()
{
    Barrier barrier1(3);
    Barrier barrier2(3);
    arrived_all = false;
    processed_all = true;
    arrived_count.store(0);
    processed_count.store(allowed_max);
    std::thread worker[50];
    for (int i = 0; i < 6; i++)
        worker[i] = std::thread(worker_thread, i, &barrier1, &barrier2);

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initialing first thread batch " << data << std::endl;
    }


    // wait until worker dies finishes execution
    for (int i = 0; i < 6; i++)
        worker[i].join();

 //    for (int i = 5; i < 10; i++)
 //        worker[i] = std::thread(worker_thread, i);

 //    {
 //        std::lock_guard<std::mutex> lk(m);
    //     std::cout << "Back in main(), after initialing second thread batch." <<  std::endl;
    // }

 //    // wait until worker dies finishes execution
 //    for (int i = 5; i < 10; i++)
 //        worker[i].join();
    std::cout << "finished main(), data = " << data << std::endl;
}

