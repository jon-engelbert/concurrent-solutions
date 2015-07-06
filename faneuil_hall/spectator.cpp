#include "Spectator.h"
#include "Judge.h"
#include <string>
#include <random>
#include <chrono>

Spectator::Spectator(size_t index) : m_index(index) {
}


void Spectator::SetJudge(std::shared_ptr<const Judge> judge) {
    // const std::shared_ptr<const Judge> m_judge;
	m_judge = judge;
}

void Spectator::Enter() {
    if (m_judge)
        m_judge->WaitForNotEntered();
    // std::unique_lock<std::mutex> lk(m);
    // // hold while judge is present.
    // // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
    // Judge::cv_judgePresent.wait(lk, [this] {return !m_judge || !m_judge->IsPresent();});
    // Enter();
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Spectator Enter: "  << m_index  <<  std::endl;
};

void Spectator::SitDown() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Spectator Sit Down: "  << m_index  <<  std::endl;
};

void Spectator::Spectate() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Spectator Spectating: "  << m_index  <<  std::endl;
};


void Spectator::Leave() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Spectator Leave: "  << m_index << std::endl;
}


void Spectator::Wait_Random() {
    std::mt19937_64 eng{std::random_device{}()};  //  seed 
    std::uniform_int_distribution<> dist{1, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
}

void Spectator::RunThread() {
	Enter();
    Spectate();
    Wait_Random();
    Leave();	
}