// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
#include <thread>
#include <iostream>
#include <vector>
#include <exception>

struct Counter {
protected:
    int m_value;
public:
    Counter() : m_value(0) {}
    int getCount() {return m_value;}
    void setCount(int val) {m_value = val;}

    void increment(){
        ++m_value;
    }
    void decrement(){
        if(m_value == 1)
            throw "Value cannot be less than 0";
        --m_value;
    }
};

struct ConcurrentCounter {
    std::mutex mutex;
    Counter counter;

    int getCount() {return counter.getCount();}
    void setCount(int val) {counter.setCount(val);}

    void increment(){
        mutex.lock();
        counter.increment();
        mutex.unlock();
    }

    void decrement(){
        mutex.lock();
        counter.decrement();        
        mutex.unlock();
    }
};

struct ConcurrentSafeCounter {
    std::mutex mutex;
    Counter counter;

    int getCount() {return counter.getCount();}
    void setCount(int val) {counter.setCount(val);}

    void increment(){
        std::lock_guard<std::mutex> guard(mutex);
        try {
        	counter.increment();
        } catch (std::exception& e) {
        	std::cout << "error caught"<< std::endl;
        }
    }

    void decrement(){
        std::lock_guard<std::mutex> guard(mutex);
        try {
        	counter.decrement();
        } catch (std::exception& e) {
        	std::cout << "error caught"<< std::endl;
        }
    }
};

int loop_parallel_up(ConcurrentSafeCounter& counter){
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&counter](){
            for(int i = 0; i < 100; ++i){
                counter.increment();
            }
        }));
    }
    for(auto& thread : threads){
        thread.join();
    }
    return counter.getCount();
}

int loop_parallel_down(ConcurrentSafeCounter& counter){
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&counter](){
            for(int i = 0; i < 100; ++i){
                counter.decrement();
            }
        }));
    }
    for(auto& thread : threads){
        thread.join();
    }
    return counter.getCount();
}

int main() {
	int value;
	ConcurrentSafeCounter counter;
    for (int i = 0; i < 50; i++) {
    	counter.setCount(0);
        value = loop_parallel_up(counter);
        if (value != 500)
            std::cout << "up mutex: " << value << std::endl;
    }
    for (int i = 0; i < 50; i++) {
    	counter.setCount(500);
        value = loop_parallel_down(counter);
        if (value != 0)
            std::cout << "down mutex: " << value << std::endl;
    }
    return 0;
}