#include "Spectator.h"
#include "Judge.h"
#include <string>
#include <random>
#include <chrono>

extern std::mutex m;

Spectator::Spectator(size_t index) : m_index(index) {
}
 
Spectator::Spectator(size_t index, std::shared_ptr<const Judge> judge) : m_index(index), m_judge(judge) {
}

// void Spectator::SetJudge(std::shared_ptr<const Judge> judge) {
//     // const std::shared_ptr<const Judge> m_judge;
// 	m_judge = judge;
// }

bool Spectator::Enter() {
    bool is_entered = false;
    if (m_judge)
        is_entered = m_judge->WaitForNotEntered();
    // Enter();
    if (is_entered) {
    // std::unique_lock<std::mutex> lk(m);
    // // hold while judge is present.
    // // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
    // Judge::cv_judgePresent.wait(lk, [this] {return !m_judge || !m_judge->IsPresent();});
    // Enter();
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Spectator Enter: "  << m_index  <<  std::endl;
    } else {
        std::lock_guard<std::mutex> lk(m);
        std::cerr << "WaitForNotEntered timed out, spectator #: " << m_index << '\n';  
    }
    return is_entered;
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
	if (Enter()) {
        Spectate();
        Wait_Random();
        Leave();
    }	
}