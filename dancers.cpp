// clang++ -std=c++14 dancers.cpp -o dancers.exe
// works!
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex m, m_rendezvous, barrier_l, barrier_f;
std::condition_variable cv_l, cv_f, cv_rendezvous;
std::string data;
std::atomic_int n_l_ready(0);
std::atomic_int n_f_waiting(0);
std::atomic_bool is_leader_preparing(false);
bool l_arrived(false);
bool f_arrived(false);
// std::atomic_bool is_arriving, is_processing, is_ready_to_arrive, is_ready_to_process;
// std::atomic_int arrived_count, processed_count;
// int allowed_max = 3;


void l_thread(int i)
{

    std::unique_lock<std::mutex> lk(barrier_l);
    // wait until a follower arrives... go through if a follower is waiting
    cv_l.wait(lk, [] {bool end_wait = (n_f_waiting > 0); return end_wait;});
    n_l_ready++;
    cv_f.notify_one();
    // rendezvous.lock();
    {
        std::unique_lock<std::mutex> lk(m_rendezvous);
        l_arrived = true;
        cv_rendezvous.wait(lk, []{return f_arrived;});
        cv_rendezvous.notify_one();
        n_l_ready--;
        f_arrived = false;
        std::cout << "leader dancing, number: " << i  << std::endl;
    }
}

void f_thread(int i)
{

    std::unique_lock<std::mutex> lk(barrier_f);
    // notify leader that you've arrived, then wait until the leader is ready.
    n_f_waiting++;
    cv_l.notify_one();
    cv_f.wait(lk, [] {bool end_wait = (n_l_ready > 0 ); return end_wait;});
    n_f_waiting--;
    {
        std::unique_lock<std::mutex> lk(m_rendezvous);
        f_arrived = true;
        cv_rendezvous.wait(lk, []{return l_arrived;});
        cv_rendezvous.notify_one();
        l_arrived = false;
        std::cout << "follower dancing, number: " << i  << std::endl;
    }
}

int main()
{
    std::thread l[50], f[50];
    for (int i = 0; i < 6; i++) {
        l[i] = std::thread(l_thread, i);
        f[i] = std::thread(f_thread, i);
    }

    {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Back in main(), after initialing first thread batch " << data << std::endl;
    }


    // wait until worker dies finishes execution
    for (int i = 0; i < 6; i++) {
        l[i].join();
        f[i].join();
        // dance(l[i], f[i]);
    }

    std::cout << "finished main()"<< std::endl;
}

