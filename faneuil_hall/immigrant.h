#ifndef IMMIGRANT_H
#define IMMIGRANT_H
#include <memory>
#include <string>
#include <mutex>
#include "judge.h"

class Judge;
class Immigrant {
private: 
    const std::shared_ptr<const Judge> m_judge;
    // mutexes are not shared... nor are unique locks, but condition variables are shared!
    static std::atomic_int m_count, m_checkedInCount;
    size_t m_index;
public:
    static std::condition_variable cv_immigrantsAllPresent;
    Immigrant(size_t index);
    void SetJudge(std::shared_ptr<const Judge>& judge);
    void Enter();

    void SitDown();

    void GetCertificate();

    void Checkin();

    void Leave();
    static bool IsAllCheckedIn() {return m_count == m_checkedInCount;}
    void WaitRandom();
    void RunThread();
};

#endif //IMMIGRANT_H