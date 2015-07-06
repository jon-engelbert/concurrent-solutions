// clang++ -std=c++14 faneuil_hall.cpp -o faneuil_hall.exe
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <random>
#include <chrono>

std::mutex m;
// should m_present and m_confirmed be global or local to each thread?
std::mutex m_present, m_confirmed, m_pre_confirm;	
std::condition_variable cv_judge_present, cv_confirmed, cv_immigrant_present;
std::string data;
std::atomic_int n_immigrant_present(0);
std::atomic_int n_immigrant_checkedin(0);
bool is_judge_present(false);
bool is_confirmed(false);

void Enter() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Enter: "  <<  std::endl;
};

void SitDown() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Sit Down: "  <<  std::endl;
};

void getCertificate() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "getCertificate: "  <<  std::endl;
};

void Spectate() {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Spectate: "  <<  std::endl;
};

void Checkin() {
	n_immigrant_checkedin++;
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Check in: " << n_immigrant_checkedin <<  std::endl;
};

void Confirm() {
        is_confirmed= true;
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Confirm." <<  std::endl;
};

void Wait_Random() {
    std::mt19937_64 eng{std::random_device{}()};  //  seed 
    std::uniform_int_distribution<> dist{1, 100};
    std::this_thread::sleep_for(std::chrono::milliseconds{dist(eng)});
};

void immigrant_leave() {
	n_immigrant_present--;
    std::lock_guard<std::mutex> lk(m);
    std::cout << "Immigrant Leave: "  << n_immigrant_present << std::endl;
}

void judge_leave() {
    is_judge_present = false;
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Judge leave." <<  std::endl;
}

void spectator_leave() {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Spectator leave." <<  std::endl;
}

void spectator_thread(int i)
{
    {
        std::unique_lock<std::mutex> lk(m_present);
        // hold while  judge is present.
        // proceed (end the barrier) before the judge arrives, or when the judge leaves
        cv_judge_present.wait(lk, [] {return !is_judge_present;});
        Enter();
    }
    Spectate();
    Wait_Random();
    spectator_leave();
}

void immigrant_thread(int i)
{
    {
        std::unique_lock<std::mutex> lk(m_present);
        // hold while judge is present.
        // proceed (end the barrier) before the judge arrives, ** and when the previous set of immigrants has left? **
        cv_judge_present.wait(lk, [] {return !is_judge_present;});
        Enter();
        n_immigrant_present++;
    }
    Checkin();
    cv_immigrant_present.notify_one();
    SitDown();
    {
        std::unique_lock<std::mutex> lk(m_confirmed);
        // hold until  judge is done confirming.
        // proceed (end the barrier) when the judge is done confirming
        cv_confirmed.wait(lk, [] {return is_confirmed;});
        getCertificate();
    }
    {
        std::unique_lock<std::mutex> lk(m_present);
        // hold while  judge is present.
        // proceed (end the barrier) after the judge leaves
        cv_judge_present.wait(lk, [] {return !is_judge_present;});
        immigrant_leave();
    }
}

void judge_thread(int i)
{
    {
        std::unique_lock<std::mutex> lk(m_present);
        // hold while judge is present.
        // proceed (end the barrier) before the judge arrives, or when the judge leaves
        // But What if two judges waiting?  can both enter at the same time?  
        // or will m_present/cv_present be locked as soon as one enters, until the end of the block, then it gets unlocked again
        cv_judge_present.wait(lk, [] {return !is_judge_present;});
        Enter();
        is_judge_present= true;
        cv_judge_present.notify_all();
    }
    {
        std::unique_lock<std::mutex> lk(m_pre_confirm);
        // hold while judge is present.
        // proceed (end the barrier) before the judge arrives, or when the judge leaves
        // But What if two judges waiting?  can both enter at the same time?  
        // or will m_present/cv_present be locked as soon as one enters, until the end of the block, then it gets unlocked again
        cv_immigrant_present.wait(lk, [] {return n_immigrant_present == n_immigrant_checkedin;});
        Confirm();
        cv_confirmed.notify_all();
    }
    judge_leave();
    // reset the cv for the judge being present, so the next iteration of the whole process can start again.
    cv_judge_present.notify_all();
}

int main()
{
	int i;
    std::thread spectator[50], immigrant[50], judge;
    for (int i = 0; i < 6; i++)
        spectator[i] = std::thread(spectator_thread, i);
    for (int i = 0; i < 6; i++)
        immigrant[i] = std::thread(immigrant_thread, i);
    // for (int i = 0; i < 2; i++)
    judge = std::thread(judge_thread, i);

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initialing  thread batch "  << std::endl;
    }
    for (int i = 0; i < 6; i++) {
        spectator[i].join();
        immigrant[i].join();
    }
    // for (int i = 0; i < 2; i++)
    judge.join();
}
