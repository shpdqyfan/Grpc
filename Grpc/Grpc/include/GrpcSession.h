/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc session
*/

#ifndef GRPC_SESSION_H
#define GRPC_SESSION_H

#include <string>

#include "Thread/Thread.h"
#include "Semaphore/Semaphore.h"
#include "GrpcClientInfo.h"

class GrpcSession : public MyThread
{
public:
    enum Status
    {
        INIT = 0,
        ENABLED = 1,
        CLOSED = 2
    };
    
    GrpcSession(const GrpcClientInfo& clientInfo);
    ~GrpcSession();
    
    void init();
    void close();
    
    const std::string getSessionId() const;
    const GrpcClientInfo& getClientInfo() const;
    const Status getStatus() const;
    void increaseCounter();
    void decreaseCounter();

protected:
    virtual void run();

private:
    GrpcClientInfo myClientInfo;
    unsigned int myCounter;
    Semaphore mySemaphore;
    Semaphore myCounterSemaphore;
    Status myStatus;
};

#endif

