#include <thread>
#include <iostream>
#include <vector>

template <typename A_Type> class Counter {
protected:
    A_Type value;
public:
    Counter() {
        value = 0;
    };
    void setValue(A_Type& val) {value.store(val);}
    A_Type& getValue() {return value;}
    void increment(){
        ++value;
    }
    void decrement(){
        --value;
        if (value < 0) {
            std::cout << "value cannot be less than 0" << std::endl;
            throw std::invalid_argument( "value cannot be less than 0" );
        }
    }
};



// There is nothing new here. Now, let's start some threads and make some increments:
template <typename T>
int loop_parallel_up(Counter<T>& counter){
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
    return counter.getValue();
}

template <typename T>
int loop_parallel_down(Counter<T>& counter, char* dataType){
    std::vector<std::thread> threads;
    for(int i = 0; i < 5; ++i){
        threads.push_back(std::thread([&counter]{
            for(int i = 0; i < 100; ++i){
                try {
                    counter.decrement();
                } catch (std::invalid_argument& e) {
                    std::cout <<  "error: " << e.what() << std::endl;
                }
            }
        }));
    }
    int i2 = 0;
    for(auto& thread : threads){
        thread.join();
        i2++;
    }
    return counter.getValue();
}

int main(){
    int value = 0;
    Counter<int> counter;
    Counter<std::atomic_int> atomic_counter;
    for (int i = 0; i < 50; i++) 
        value = loop_parallel_up(counter);
    try {
        std::atomic_int atomic_val;
        atomic_val.store(value);
        atomic_counter.setValue(atomic_val);
        for (int i = 0; i < 50; i++) 
            value = loop_parallel_down(atomic_counter, (char*)"atomic_int");
    } catch (const char& e) {
        std::cout <<"int" << e << std::endl;
    }

    for (int i = 0; i < 50; i++) 
        value = loop_parallel_up(atomic_counter);

    for (int i = 0; i < 50; i++) 
        value = loop_parallel_down(atomic_counter, (char*)"atomic_int");


    return 0;
}