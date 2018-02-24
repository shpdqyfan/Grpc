/*
    Author: shpdqyfan
    profile: A simple semaphore which based on mutex and 
               condition variable of cplusplus 11. Because of no
               semaphore provided by cplusplus 11 standard lib.
*/

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
    Semaphore(uint32_t semCount);
    ~Semaphore();

    void wait();
    bool waitFor(uint32_t millsec);
    void notify();

private:
    uint32_t semaphoreCount;
    std::mutex myMutex;
    std::condition_variable myCondVar;
};

#endif

