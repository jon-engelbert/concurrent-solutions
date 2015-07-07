#ifndef JUDGE_H
#define JUDGE_H
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>


class Judge {
private: 
	bool m_isDoneConfirming;
	static bool m_isPresent;
    static std::mutex m_presentMutex;
    static std::mutex m_preconfirmMutex;
    size_t m_index;
public:
	static std::condition_variable cv_confirmed;
	static std::condition_variable cv_judgePresent;
	Judge() {};
	Judge(size_t index);
    void Enter();

    void SitDown();

    void Confirm();

    void Wait_Random();

    void Leave();
    bool IsPresent() const {return m_isPresent;}
    bool IsDoneConfirming() {return m_isDoneConfirming;}
    void SignalImmigrantsPresent() const;
    void RunThread();
    void WaitForNotEntered() const;
	void WaitForConfirmed() const;
};

#endif // JUDGE_H