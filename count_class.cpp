#include <thread>
#include <iostream>
#include <vector>

template <typename A_Type> class Counter {
public:
    A_Type value;
    Counter() {
        value = 0;
    };
    void increment(){
        ++value;
    }
};



// There is nothing new here. Now, let's start some threads and make some increments:
template <typename T>
int loop_parallel(Counter<T>& counter){
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&counter](){
            for(int i = 0; i < 100; ++i){
                counter.increment();
            }
        }));
    }
    int i2 = 0;
    for(auto& thread : threads){
        thread.join();
        i2++;
    }
    return counter.value;
}

int main(){
    int value = 0;
    Counter<int> counter;
    for (int i = 0; i < 50; i++) {
        counter.value = 0;
        value = loop_parallel(counter);
        if (value != 500)
            std::cout << "non-atomic: " << counter.value << std::endl;
    }

    Counter<std::atomic_int> atomic_counter;
    for (int i = 0; i < 50; i++) {
        atomic_counter.value = 0;
        value = loop_parallel(atomic_counter);
        if (value != 500)
            std::cout << "atomic: " << atomic_counter.value << std::endl;
    }

    return 0;
}