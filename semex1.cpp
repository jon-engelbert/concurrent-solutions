/**
 * Semaphore example, written in C++ May 4, 2014
 * Compiled on OSX 10.9, using:
 * g++ -std=c++11 semaphore.cpp
 **/

// clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
 
#include <iostream>      
#include <thread>        
#include <mutex>         
#include <condition_variable>
#include <map>
#include "semex1.hpp"
 
std::mutex mtx;             // mutex for critical section
std::condition_variable cv; // condition variable for critical section  
bool ready = false;         // Tell threads to run
int current = 0;            // current count

std::map<std::string, clock_t> FunctionTimer::mFunctionTimes = {};

FunctionTimer::~FunctionTimer() 
{ 
  mFunctionTimes[mName] += clock() - mStartTime; 
}
void FunctionTimer::printTime(std::pair<const std::string, clock_t>& profile) // could be a class static method as well
{
  printf("fn: %s, time: %Lf", profile.first.c_str(), (long double) profile.second);
} 

void FunctionTimer::report()
{
  // ... iterate through mFunctionTimes, printing out the names and accumulated ticks ...
  std::for_each(mFunctionTimes.begin(), mFunctionTimes.end(), printTime);
}

 

/* Prints the thread id / max number of threads */
void print_num(int num, int max) {
 
  std::unique_lock<std::mutex> lck(mtx);
  // while(num != current || !ready)
  //   { cv.wait(lck); }
  while(!ready)
    { cv.wait(lck); }
  current++;
  std::cout << "Thread: ";
  std::cout << num + 1 << " / " << max;
  std::cout << " current count is: ";
  std::cout << current << std::endl;
  
  /* Notify next threads to check if it is their turn */
  cv.notify_all(); 
}
 
/* Changes ready to true, and begins the threads printing */
void run(){
  FunctionTimer ft("run");
  std::unique_lock<std::mutex> lck(mtx);
  ready = true;
  cv.notify_all();
}
 
int main (){
 
  int threadnum = 100;
  std::thread threads[100];
 
  /* spawn threadnum threads */
  for (int id = 0; id < threadnum; id++)
    threads[id] = std::thread(print_num, id, threadnum);
 
  std::cout << "\nRunning " << threadnum;
  std::cout << " in parallel: \n" << std::endl;
 
  run(); // Allows threads to run
 
  /* Merge all threads to the main thread */
  for(int id = 0; id < threadnum; id++)
    threads[id].join();
 
  std::cout << "\nCompleted semaphore example!\n";
  std::cout << std::endl;
 
  FunctionTimer::report();
  return 0;
}