// clang++ -std=c++14 faneuil_hall.cpp immigrant.cpp spectator.cpp judge.cpp -o faneuil_hall.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <random>
#include <chrono>
#include "immigrant.h"
#include "spectator.h"
#include "Judge.h"

std::mutex m;
// cv_judge_present is global to all...  then we can use notify_all to notify all 
// judges, spectators and immigrants when a judge arrives or leaves.


void Wait_Random() {
    std::mt19937_64 eng{std::random_device{}()};  //  seed 
    std::uniform_int_distribution<> dist{1, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
};





int main()
{
	int i;
    // Spectator spectator_(0);
    // Immigrant immigrant_(0);
    std::vector<Spectator*> spectators;
    std::vector<Immigrant*> immigrants;
    static const int NUM_SPECTATORS = 10;
    static const int NUM_IMMIGRANTS = 10;
    std::thread spectator_thread[NUM_SPECTATORS], immigrant_thread[NUM_IMMIGRANTS];
    std::thread judge_thread;
    auto judge = std::make_shared<Judge>();
    judge_thread = std::thread(judge.RunThread());
    for (int i = 0; i < NUM_SPECTATORS; i++) {
        auto spectator = new Spectator(i);
        spectator->SetJudge(judge);
        spectators.push_back(spectator);
        spectator_thread[i] = std::thread(spectator->RunThread());
    }
    for (int i = 0; i < NUM_IMMIGRANTS; i++) {
        auto immigrant = new Immigrant(i);
        immigrant->SetJudge(judge);
        immigrant_thread[i] = std::thread(immigrant->RunThread());
        immigrants.push_back(immigrant);
    }
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initializing  thread batch "  << std::endl;
    }
    for (int i = 0; i < 6; i++) {
        spectator_thread[i].join();
        immigrant_thread[i].join();
    }
    // for (int i = 0; i < 2; i++)
    judge_thread.join();
}
