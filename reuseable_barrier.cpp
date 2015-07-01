// http://en.cppreference.com/w/cpp/thread/condition_variable
// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
// works!
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, barrier1, barrier2, m_wait;
std::condition_variable cv1, cv2;
std::string data;
std::atomic_bool is_arriving, is_processing, is_ready_to_arrive, is_ready_to_process;
std::atomic_int arrived_count, processed_count;
int allowed_max = 3;


void worker_thread(int i)
{

    {
        std::unique_lock<std::mutex> lk2(barrier2);
        // hold while NOT arriving or less than allowed number have arrived.
        // proceed (end the barrier) when arriving (or ready to arrive)
        cv2.wait(lk2, [] {bool end_wait = (is_arriving || is_ready_to_arrive) && (arrived_count < allowed_max); return end_wait;});
        arrived_count++; processed_count--; 
        is_arriving = true; is_processing = false; 
        is_ready_to_arrive = false; is_ready_to_process = false;
    }
    {
        std::lock_guard<std::mutex> lk_guard(m);
        std::cout << "counts changing1, is_arriving: " << is_arriving << ".  processed: " << processed_count << ", arrived :"  << arrived_count << " number: " << i  << std::endl;
    }
    // cv2.notify_all();
    // lk2.unlock();

    {
        std::lock_guard<std::mutex> lk_guard(m);
        // arrived_count++;
        // processed_count--;
        std::cout << "counts changing2, is_arriving: " << is_arriving << ".  processed: " << processed_count << ", arrived:" << arrived_count << " number: " << i << std::endl;
    }

    {
        std::lock_guard<std::mutex> lk_guard(m);
        if (arrived_count >= allowed_max) {
            std::cout << "Enough arrived, about to open floodgate #1" << std::endl;
            is_ready_to_process = true;
            for (int i = 0; i < allowed_max; i++) {
                cv1.notify_one();
            }
        }
        std::cout << "about to reach 2nd barrier:" << arrived_count << " number: " << i << std::endl;

    }
    std::unique_lock<std::mutex> lk1(barrier1);
    {
        // std::lock_guard<std::mutex> lk_guard(m_wait);
        cv1.wait(lk1, []{return (processed_count < allowed_max) && (is_processing || is_ready_to_process);});
        processed_count++; arrived_count--; is_arriving = false; is_processing = true; is_ready_to_arrive = false; is_ready_to_process = false;
        // processed_count++;
        // arrived_count--;
    }
    // cv1.notify_all();
    lk1.unlock();

    // critical section would go here... then increment processed_count

    // report after critical section
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread "  << i << " data processing completed" << std::endl;
        if (processed_count >= allowed_max) {
            std::cout << "Group finished, about to open floodgate #2" << std::endl;
            is_ready_to_arrive = true;
            for (int i = 0; i < allowed_max; i++) {
                cv2.notify_one();
            }
        }
    }
}

int main()
{
    is_arriving = true;
    is_processing = false;
    is_ready_to_arrive = false;
    is_ready_to_process = false;
    arrived_count = 0;
    processed_count = allowed_max;
    std::thread worker[50];
    for (int i = 0; i < 6; i++)
        worker[i] = std::thread(worker_thread, i);

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initialing first thread batch " << data << std::endl;
    }


    // wait until worker dies finishes execution
    for (int i = 0; i < 6; i++)
        worker[i].join();

    for (int i = 6; i < 12; i++)
        worker[i] = std::thread(worker_thread, i);

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initialing second thread batch." <<  std::endl;
    }

    // wait until worker dies finishes execution
    for (int i = 6; i < 12; i++)
        worker[i].join();
    std::cout << "finished main(), data = " << data << std::endl;
}

