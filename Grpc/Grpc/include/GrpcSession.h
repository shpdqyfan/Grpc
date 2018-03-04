/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc session
*/

#ifndef GRPC_SESSION_H
#define GRPC_SESSION_H

#include <string>
#include <memory>

#include "Thread/Thread.h"
#include "Semaphore/Semaphore.h"
#include "GrpcClientInfo.h"

class EasyTimer;
class GrpcSessionMgr;

static const int GRPC_SESSION_INACTIVITY_TIME = 3*1000; //for easy to trigger testing case

class GrpcSession : public MyThread
{
public:
    enum Status
    {
        INIT = 0,
        ENABLED = 1,
        CLOSED = 2
    };
    
    GrpcSession(const GrpcClientInfo& clientInfo,
        std::shared_ptr<EasyTimer> timerMgr, GrpcSessionMgr* sessionMgr);
    ~GrpcSession();
    
    void init();
    void close(const std::string& reason);
    
    const std::string getSessionId() const;
    const GrpcClientInfo& getClientInfo() const;
    const Status getStatus() const;
    void restartTimer();
    void increaseReqCounter();
    void decreaseReqCounter();

protected:
    virtual void run();

private:
    GrpcClientInfo myClientInfo;
    //one session may has multiple subscribe requests
    unsigned int myReqCounter;
    Semaphore myShutdownSemaphore;
    Semaphore myReqCounterSemaphore;
    Status myStatus;
    std::shared_ptr<EasyTimer> myInActivityTimerMgr;
    GrpcSessionMgr* mySessionManagerPtr;
};

#endif

