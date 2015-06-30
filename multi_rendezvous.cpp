// http://en.cppreference.com/w/cpp/thread/condition_variable
// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, mi;
std::condition_variable cv;
std::string data;
std::atomic_bool arrived_all;
std::atomic_int thread_count, arrived_count;


void worker_thread(int i)
{
    {
        // std::lock_guard<std::mutex> lk(m);
        std::lock_guard<std::mutex> lk(m);
        data += " initial processing ";
        data += char(i);
        std::cout << "Worker thread "  << i << " is in step a: " << data << std::endl;
    }
    arrived_count++;
    if (arrived_count == thread_count)
        arrived_all = true;
    {
        std::unique_lock<std::mutex> lk(mi);
        cv.wait(lk, []{return arrived_all.load();});
        cv.notify_all();
    }

    // Send data back to main()
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread "  << i << " data processing completed\n";
        data += "worker thread complete:  " ;
        data += char(i);
    }
}

int main()
{
    arrived_all = false;
    thread_count = 10;
    std::thread worker[10];
    for (int i = 0; i < 10; i++)
        worker[i] = std::thread(worker_thread, i);

    data = "Example data";
    std::cout << "Back in main(), data = " << data << '\n';


    // wait until worker dies finishes execution
    for (int i = 0; i < 10; i++)
        worker[i].join();
    std::cout << "finished main(), data = " << data << '\n';
}