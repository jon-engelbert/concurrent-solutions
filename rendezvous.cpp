// http://en.cppreference.com/w/cpp/thread/condition_variable
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, m1, m2;
std::condition_variable cv1, cv2;
std::string data;
bool arrived1, arrived2 = false;

void worker_thread1()
{
    {
        std::lock_guard<std::mutex> lk(m);
        data += " initial processing 1.  ";
        std::cout << "Worker thread 1 is in step a: " << data << std::endl;
        arrived1 = true;
    }

    {
        std::unique_lock<std::mutex> lk(m1);
        cv1.wait(lk, []{return arrived2;});
        cv2.notify_one();
    }


    // Send data back to main()
    data += "worker thread 1 complete.  ";
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread 1 data processing completed\n";
    }
}

void worker_thread2()
{
    {
        // std::lock_guard<std::mutex> lk(m);
        std::lock_guard<std::mutex> lk(m);
        data += " initial processing 2.  ";
        std::cout << "Worker thread 2 is in step a: " << data << std::endl;
        arrived2 = true;
    }
    {
        std::unique_lock<std::mutex> lk(m2);
        cv2.wait(lk, []{return arrived1;});
        cv1.notify_one();
    }

    // Send data back to main()
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread 2 data processing completed\n";
        data += "worker thread 2 complete.  ";
    }
}

int main()
{
    std::thread worker1(worker_thread1);
    std::thread worker2(worker_thread2);

    data = "Example data";
    std::cout << "Back in main(), data = " << data << '\n';


    // wait until worker dies finishes execution
    worker1.join();
    worker2.join();
    std::cout << "finished main(), data = " << data << '\n';
}