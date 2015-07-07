#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <random>
#include <chrono>

#include "judge.h"
#include "immigrant.h"

std::condition_variable Judge::cv_confirmed;
std::condition_variable Judge::cv_judgePresent;
std::condition_variable Judge::cv_immigrantsAllPresent;
std::mutex Judge::m_presentMutex;
std::mutex Judge::m_preconfirmMutex;
bool Judge::m_isPresent = false;
extern std::mutex m;

Judge::Judge(size_t index) : m_index(index), m_isDoneConfirming(false), m_hasEntered(false) {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Judge initialized: "  <<  std::endl;
}


void Judge::SignalImmigrantsPresent() const {
	cv_immigrantsAllPresent.notify_one();
}

void Judge::Enter() {
    {
        std::unique_lock<std::mutex> lk(m_presentMutex);
        // hold while judge is present.
        // proceed (end the barrier) before the judge arrives, or when the judge leaves
        // But What if two judges waiting?  can both enter at the same time?  
        // or will m_present/cv_present be locked as soon as one enters, until the end of the block, then it gets unlocked again
        cv_judgePresent.wait(lk, [this] {return !m_isPresent;});
    // }
    // {
    //     std::lock_guard<std::mutex> lk(m);
        std::cout << "Judge Enter: "  <<  std::endl; 
        // std::lock_guard<std::mutex> lk(m);
        m_isPresent = true;
        m_hasEntered = true;
        cv_judgePresent.notify_all();	// notify all judges, immigrants and spectators.
    }
};


void Judge::SitDown() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Judge Sit Down: "  <<  std::endl;
};


void Judge::Confirm() {
    {
        std::unique_lock<std::mutex> lk(m_preconfirmMutex);
        // hold while judge is present.
        // proceed (end the barrier) before the judge arrives, or when the judge leaves
        // But What if two judges waiting?  can both enter at the same time?  
        // or will m_present/cv_present be locked as soon as one enters, until the end of the block, then it gets unlocked again
        cv_immigrantsAllPresent.wait(lk, [] {return Immigrant::IsAllCheckedIn();});
        m_isDoneConfirming = true;
    }
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Judge Confirm." <<  std::endl;
    }
    cv_confirmed.notify_all();
};


void Judge::Leave() {
    m_isPresent = false;
    // m_isDoneConfirming = false;
    std::mutex presentMutex;
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Judge leave." <<  std::endl;
    cv_judgePresent.notify_all();   // notify all judges, immigrants and spectators.
}


// void Wait_Random() {
//     std::mt19937_64 eng{std::random_device{}()};  //  seed 
//     std::uniform_int_distribution<> dist{1, 100};
//     std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
// };

void Judge::RunThread()
{
    Enter();
    Confirm();
    Leave();
    // reset the cv for the judge being present, so the next iteration of the whole process can start again.
    cv_judgePresent.notify_all();
}

bool Judge::WaitForNotEntered() const {
    bool is_entered = false;
    std::unique_lock<std::mutex> lk(m_presentMutex);
    // hold while judge is present.
    // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
    auto now = std::chrono::system_clock::now();
    if (cv_judgePresent.wait_until(lk, now + std::chrono::milliseconds(100), [this] {return !IsPresent() && !m_hasEntered;})) 
        is_entered = true;  
    return is_entered;
}

void Judge::WaitForConfirmed() const
{
    std::unique_lock<std::mutex> lock(m_preconfirmMutex);
    cv_confirmed.wait(lock, [this] { return m_isDoneConfirming; });
}

