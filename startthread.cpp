#include <thread>
#include <iostream>
#include <vector>

std::mutex mutex;

void hello(){
	mutex.lock();
    std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
    mutex.unlock();
}

int main(){
    std::vector<std::thread> threads;

    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread(hello));
    }

    for(auto& thread : threads){
        thread.join();
    }

    return 0;
}