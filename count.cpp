#include <thread>
#include <iostream>
#include <vector>



template <typename T>
int loop_parallel(T& count){
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&count](){
            for(int i = 0; i < 100; ++i){
                count++;
            }
        }));
    }
    int i2 = 0;
    for(auto& thread : threads){
        thread.join();
        i2++;
    }
    return count;
}

int main(){
    int value = 0;
    int count;
    for (int i = 0; i < 50; i++) {
        count = 0;
        value = loop_parallel(count);
        if (value != 500)
            std::cout << "non-atomic: " << value << std::endl;
    }

    std::atomic_int atomic_count;
    for (int i = 0; i < 50; i++) {
        atomic_count = 0;
        value = loop_parallel(atomic_count);
        if (value != 500)
            std::cout << "atomic: " << value << std::endl;
    }

    return 0;
}