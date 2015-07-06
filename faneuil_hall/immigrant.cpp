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

Immigrant::Immigrant(size_t index) : m_index(index) {
}

void Immigrant::SetJudge(std::shared_ptr<const Judge>& judge) {
    m_judge =judge;
}

void Immigrant::Enter() {
    // std::unique_lock<std::mutex> lk(m);
    // hold while judge is present.
    // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
    // Judge::cv_judgePresent.wait(lk, [this] {return !m_judge || !m_judge->IsPresent();});
    if (m_judge)
        m_judge->WaitForNotEntered();
    // Enter();
    std::mutex m;
    std::lock_guard<std::mutex> lk(m);
    m_count++;
    std::cout << "Immigrant Enter: "  <<  std::endl;
};

void Immigrant::SitDown() {
    std::mutex m;
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Immigrant Sit Down: "  <<  std::endl;
};

void Immigrant::GetCertificate() {
    std::mutex m_confirmed;
    std::unique_lock<std::mutex> lk(m_confirmed);
    // hold until  judge is done confirming.
    // proceed (end the barrier) when the judge is done confirming
    if (m_judge)
        m_judge->WaitForConfirmed();
    // Judge::cv_confirmed.wait(lk, [] {return Judge::IsDoneConfirming();});
    std::cout << "getCertificate: "  <<  std::endl;
};

void Immigrant::Checkin() {
	m_checkedInCount++;
    std::mutex m;
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Check in: " << m_checkedInCount <<  std::endl;
    if (m_judge && IsAllCheckedIn())
        m_judge->SignalImmigrantsPresent();
};

void Immigrant::Leave() {
    // std::unique_lock<std::mutex> lk(m_present);
    // // hold while  judge is present.
    // // proceed (end the barrier) after the judge leaves
    // Judge::cv_judgePresent.wait(lk, [this] {return !m_judge->IsPresent();});
    if (m_judge)
        m_judge->WaitForNotEntered();
    std::mutex m;
    std::lock_guard<std::mutex> lk(m);
    m_count--;
    m_checkedInCount--;
    std::cout << "Immigrant Leave: "  << m_count << std::endl;
}


void Immigrant::WaitRandom() {
    std::mt19937_64 eng{std::random_device{}()};  //  seed 
    std::uniform_int_distribution<> dist{1, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
}

void Immigrant::RunThread() {
    Enter();
    Checkin();
    // cv_immigrant_present.notify_one();
    SitDown();
    GetCertificate();
    Leave();
}
