// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m;
std::condition_variable cv;
std::string data;
bool mainReady = false;
bool workerReady = false;

void worker_thread()
{
    // Wait until main() sends data
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return mainReady;});
    }

    std::cout << "Worker thread is processing data: " << data << std::endl;
    data += " after processing";

    // Send data back to main()
    {
        std::lock_guard<std::mutex> lk(m);
        workerReady = true;
        std::cout << "Worker thread signals data processing completed\n";
    }
    cv.notify_one();
}

int main()
{
    std::thread worker(worker_thread);

    data = "Example data";
    // send data to the worker thread
    {
        std::lock_guard<std::mutex> lk(m);
        mainReady = true;
        std::cout << "main() signals data ready for processing\n";
    }
    cv.notify_one();

    // wait for the worker
    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return workerReady;});
    }
    std::cout << "Back in main(), data = " << data << '\n';


    // wait until worker dies finishes execution
    worker.join();
}