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
    , myCounter(0)
    , mySemaphore(0)
    , myCounterSemaphore(0)
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
        mySemaphore.notify();
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

void GrpcSession::increaseCounter()
{
    myCounter++;
}

void GrpcSession::decreaseCounter()
{
    myCounter--;
    myCounterSemaphore.notify();
}

void GrpcSession::run()
{
    setRunningState(RUNNING);
    myStatus = ENABLED;

    std::cout<<"GrpcSession, running, sessionId="<<getSessionId()<<std::endl;

    while(RUNNING == getRunningState())
    {
        mySemaphore.wait();
    }

    while(0 < myCounter)
    {
        myCounterSemaphore.waitFor(1000);
    }

    std::cout<<"GrpcSession, running end, sessionId="<<getSessionId()<<std::endl;
}


