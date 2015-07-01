// http://en.cppreference.com/w/cpp/thread/condition_variable
// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
// not working (yet!)
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, barrier1, barrier2;
std::condition_variable cv1, cv2;
std::string data;
std::atomic_bool arrived_all, processed_all;
std::atomic_int arrived_count, processed_count;
int allowed_max = 3;


void worker_thread(int i)
{
    {
        // std::lock_guard<std::mutex> lk(m);
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread "  << i << " has arrived." << std::endl;
    }
    {
	    arrived_count++;
    }
    if (arrived_count == allowed_max) {
        std::cout << "All arrived, about to open floodgate #1" << std::endl;
    	barrier2.lock();
    	barrier1.unlock();
    }
    {
        barrier1.lock();
    	barrier2.unlock();
    	arrived_count--;
    }


    // report after critical section
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Worker thread "  << i << " data processing completed" << std::endl;
	    processed_count++;
    }
	    if (processed_count == allowed_max) {
	        std::cout << "Group finished, about to open floodgate #2" << std::endl;
	        barrier1.lock();
        	barrier2.unlock();
	    }
    {
        	barrier2.lock();
        	barrier1.unlock();
        	processed_count--;
    }
    // critical section would go here... then increment processed_count
}

int main()
{
	barrier1.lock();
	barrier2.unlock();
	arrived_count = 0;
    processed_count = 0;
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