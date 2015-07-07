// clang++ -std=c++14 faneuil_hall.cpp -o faneuil_hall.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <random>
#include <chrono>

#include "immigrant.h"
#include "judge.h"

extern std::mutex m;

std::atomic_int Immigrant::m_count, Immigrant::m_checkedInCount;
int Immigrant::m_maxCount = 0;

Immigrant::Immigrant(size_t index) : m_index(index) {
}

Immigrant::Immigrant(size_t index, std::shared_ptr<const Judge> judge) : m_index(index), m_judge(judge) {
}

// void Immigrant::SetJudge(std::shared_ptr<const Judge>& judge) {
//     m_judge =judge;
// }

bool Immigrant::Enter() {
    // std::unique_lock<std::mutex> lk(m);
    // hold while judge is present.
    // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
    // Judge::cv_judgePresent.wait(lk, [this] {return !m_judge || !m_judge->IsPresent();});
    bool is_entered = false;
    if (m_judge)
        is_entered = m_judge->WaitForNotEntered();
    // Enter();
    if (is_entered) {
        std::lock_guard<std::mutex> lk(m);
        m_count++;
        std::cout << "Immigrant Enter: " << m_index <<  std::endl;
    } else {
        std::lock_guard<std::mutex> lk(m);
        std::cerr << "WaitForNotEntered timed out, immigrant #: " << m_index << '\n';  
    }
    return is_entered;
};

void Immigrant::SitDown() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Immigrant Sit Down: "  << m_index <<  std::endl;
};

void Immigrant::GetCertificate() {
    // std::mutex m_confirmed;
    // std::unique_lock<std::mutex> lk(m_confirmed);
    // hold until  judge is done confirming.
    // proceed (end the barrier) when the judge is done confirming
    if (m_judge)
        m_judge->WaitForConfirmed();
    // Judge::cv_confirmed.wait(lk, [] {return Judge::IsDoneConfirming();});

    std::lock_guard<std::mutex> lk(m);
    std::cout << "getCertificate: "  << m_index <<  std::endl;
};

void Immigrant::Checkin() {
	m_checkedInCount++;
    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Check in: " << m_index << ".  checked in count: "<< m_checkedInCount <<  std::endl;
    }
    // if (m_judge && IsAllCheckedIn())
    if (m_judge)
        m_judge->SignalImmigrantsPresent();
};

void Immigrant::Leave() {
    {
        std::unique_lock<std::mutex> lk(Judge::m_presentMutex);
        // // hold while  judge is present.
        // // proceed (end the barrier) after the judge leaves
        Judge::cv_judgePresent.wait(lk, [this] {return !m_judge || !m_judge->IsPresent();});
        m_count--;
        m_checkedInCount--;
     }
        // if (m_judge)
        //     m_judge->WaitForNotEntered();
     {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Immigrant Leave: "  << m_index << std::endl;
    }
}


void Immigrant::WaitRandom() {
    std::mt19937_64 eng{std::random_device{}()};  //  seed 
    std::uniform_int_distribution<> dist{1, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
}

void Immigrant::RunThread() {
    bool is_entered = Enter();
    if (is_entered) {
        Checkin();
        // cv_immigrant_present.notify_one();
        SitDown();
        GetCertificate();
        Leave();
    }
}
