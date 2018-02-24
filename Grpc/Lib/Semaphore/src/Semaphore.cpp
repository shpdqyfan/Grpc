/*
    Author: shpdqyfan
    profile: A simple semaphore which based on mutex and 
               condition variable of cplusplus 11. Because of no
               semaphore provided by cplusplus 11 standard lib.
*/

#include <thread>
#include <chrono>
#include <iostream>

#include "Semaphore/Semaphore.h"

Semaphore::Semaphore(uint32_t semCount)
    : semaphoreCount(semCount)
{
    std::cout<<"Semaphore, construct, semaphoreCount="<<semaphoreCount<<std::endl;
}

Semaphore::~Semaphore()
{
    std::cout<<"Semaphore, deconstruct"<<std::endl;
}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> guard(myMutex);

    while(0 == semaphoreCount)
    {
        std::cout<<"Semaphore in thread="<<std::this_thread::get_id()<<", start waiting"<<std::endl;
        myCondVar.wait(guard);
        std::cout<<"Semaphore in thread="<<std::this_thread::get_id()<<", waiting done"<<std::endl;
    }
    
    semaphoreCount--;
}

bool Semaphore::waitFor(uint32_t millsec)
{
    std::unique_lock<std::mutex> guard(myMutex);

    if(0 == semaphoreCount)
    {
        std::cout<<"Semaphore in thread="<<std::this_thread::get_id()<<", start waiting for 0~"<<millsec<<std::endl;
        myCondVar.wait_for(guard, std::chrono::milliseconds(millsec));
        std::cout<<"Semaphore in thread="<<std::this_thread::get_id()<<", waiting for done"<<std::endl;

        if(0 == semaphoreCount)
        {
            std::cout<<"Semaphore in thread="<<std::this_thread::get_id()<<" waiting for error"<<std::endl;
            return false;
        }
    }

    semaphoreCount--;
    return true;
}

void Semaphore::notify()
{
    std::unique_lock<std::mutex> guard(myMutex);

    semaphoreCount++;

    std::cout<<"Semaphore, notify, count="<<semaphoreCount<<std::endl;
    
    myCondVar.notify_one();
}

