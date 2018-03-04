/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc session
*/

#include <iostream>

#include "GrpcSession.h"
#include "Timer/EasyTimer.h"

GrpcSession::GrpcSession(const GrpcClientInfo& clientInfo, 
    std::shared_ptr<EasyTimer> timerMgr)
    : myClientInfo(clientInfo)
    , myReqCounter(0)
    , myShutdownSemaphore(0)
    , myReqCounterSemaphore(0)
    , myStatus(INIT)
    , myInActivityTimerMgr(timerMgr)
{
    std::cout<<"GrpcSession, construct, sessionId="<<getSessionId()<<std::endl;
}

GrpcSession::~GrpcSession()
{ 
    std::cout<<"GrpcSession, deconstruct, sessionId="<<getSessionId()<<std::endl;
}

void GrpcSession::init()
{
    myInActivityTimerMgr->addTimer(getSessionId(), GRPC_SESSION_INACTIVITY_TIME,
        std::bind(&GrpcSession::close, this, "Session closed due to inactivity"));
}

void GrpcSession::close(const std::string& reason)
{
    std::cout<<"GrpcSession, sessionId="<<getSessionId()<<", "<<reason<<std::endl;
    
    if(RUNNING == getRunningState())
    {
        setRunningState(WAITING);
        myStatus = CLOSED;
        myShutdownSemaphore.notify();
        myInActivityTimerMgr->cancelTimer(getSessionId());
    }
}

const std::string GrpcSession::getSessionId() const
{
    std::string sId = std::string(myClientInfo.ipAddr) 
        + std::string(":") + std::string(myClientInfo.port);
    
    return sId;
}

const GrpcClientInfo& GrpcSession::getClientInfo() const
{
    return myClientInfo;
}

const GrpcSession::Status GrpcSession::getStatus() const
{
    return myStatus;
}

void GrpcSession::restartTimer()
{
    myInActivityTimerMgr->updateTimer(getSessionId(), GRPC_SESSION_INACTIVITY_TIME);
}

//TBD: Considering the possible concurrency
void GrpcSession::increaseReqCounter()
{
    myReqCounter++;
    std::cout<<"GrpcSession, sessionId="<<getSessionId()<<", after increase, myReqCounter="<<myReqCounter<<std::endl;
}

//TBD: Considering the possible concurrency
void GrpcSession::decreaseReqCounter()
{
    myReqCounter--;
    std::cout<<"GrpcSession, sessionId="<<getSessionId()<<", after decrease, myReqCounter="<<myReqCounter<<std::endl;
    myReqCounterSemaphore.notify();
}

void GrpcSession::run()
{
    setRunningState(RUNNING);
    myStatus = ENABLED;

    std::cout<<"GrpcSession, running, sessionId="<<getSessionId()<<std::endl;

    while(RUNNING == getRunningState())
    {
        myShutdownSemaphore.wait();
    }

    //waiting for all the requests of this session release 
    while(0 < myReqCounter)
    {
        myReqCounterSemaphore.waitFor(1000);
    }

    std::cout<<"GrpcSession, running end, sessionId="<<getSessionId()<<std::endl;
}


