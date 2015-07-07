#ifndef SPECTATOR_H
#define SPECTATOR_H
#include "judge.h"
class Judge;
class Spectator {
private: 
    std::shared_ptr<const Judge> m_judge;
    // Judge m_judge;
    // static std::condition_variable cv_judge_present;
    size_t m_index;
public:
	Spectator(size_t index);
	Spectator(size_t index, std::shared_ptr<const Judge> judge);
	// Spectator(std::shared_ptr<const Judge> judge);
    // void SetJudge(std::shared_ptr<const Judge> judge);
    void Enter();

    void SitDown();

    void Spectate();

    void Wait_Random();

    void Leave();
    static int GetCount();
    void RunThread();
};


#endif 