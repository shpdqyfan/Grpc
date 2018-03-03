/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc session manager
               Handles all sessions. It waits for new connection
               and create grpc session in a new thread. In the 
               end it is notified when a session ends to delete
               the grpc session.
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <iterator>

#include "GrpcSessionMgr.h"
#include "GrpcSession.h"
#include "GrpcClientInfo.h"
#include "Timer/EasyTimer.h"

GrpcSessionMgr::GrpcSessionMgr(const std::string& addr, int port)
    : myGrpcServerAddr(addr)
    , myGrpcServerPort(port)
    , myGrpcServerBuilder()
    , myGrpcService()
    , myRunning(false)
    , mySessionMap()
    , myRestartSemaphore(0)
    , myInactivityTimerMgr(new EasyTimer)
{
    std::cout<<"GrpcSessionMgr, construct"<<std::endl;
}

GrpcSessionMgr::~GrpcSessionMgr()
{ 
    std::cout<<"GrpcSessionMgr, deconstruct"<<std::endl;
    MyThread::join();
}

void GrpcSessionMgr::init()
{ 
    myGrpcService.registerGrpcSessionMgr(this);
    MyThread::start();
    myInactivityTimerMgr->start();
}

void GrpcSessionMgr::stop()
{
    std::cout<<"GrpcSessionMgr, stop"<<std::endl;
    
    std::chrono::milliseconds dur(5000);
    std::chrono::time_point<std::chrono::system_clock> dt(dur);

    myRunning = false;
    myGrpcServer->Shutdown(dt);
    myRestartSemaphore.notify();
    
    std::unique_lock<std::recursive_mutex> guard(myMutex);
    GrpcSessionMap::iterator it;
    for(it = mySessionMap.begin();it != mySessionMap.end();++it)
    {
        if(NULL != it->second)
        {
            it->second->join();
            (it->second).reset();
        }

        mySessionMap.erase(it++);
    }

    myInactivityTimerMgr->stop();
}

std::shared_ptr<GrpcSession> GrpcSessionMgr::requestSession(const GrpcClientInfo& clientInfo)
{
    std::unique_lock<std::recursive_mutex> guard(myMutex);

    std::string sessionId = std::string(clientInfo.ipAddr) 
        + std::string(":") + std::string(clientInfo.port);

    std::cout<<"GrpcSessionMgr, requestSession, sessionId="<<sessionId<<std::endl;
    
    if(!hasSession(sessionId))
    {
        createSession(clientInfo);
    }

    return getSession(sessionId);
}

void GrpcSessionMgr::run()
{
    myRunning = true;
    
    std::cout<<"GrpcSessionMgr, running"<<std::endl;
    
    while(myRunning)
    {
        initGRPCServer();        
        if(myGrpcServer.get())
        {
            myGrpcServer->Wait();
        }
        
        myRestartSemaphore.wait();
    }

    std::cout<<"GrpcSessionMgr, running end"<<std::endl;
}

void GrpcSessionMgr::initGRPCServer()
{
    std::cout<<"GrpcSessionMgr, initGRPCServer"<<std::endl;
        
    const std::string sAddr = myGrpcServerAddr + std::string(":") + std::to_string(myGrpcServerPort);
    
    myGrpcServerBuilder.AddListeningPort(sAddr, grpc::InsecureServerCredentials());
    myGrpcServerBuilder.RegisterService(&myGrpcService);
      
    myGrpcServer.reset();
    myGrpcServer = std::move(myGrpcServerBuilder.BuildAndStart());
}

void GrpcSessionMgr::createSession(const GrpcClientInfo& clientInfo)
{
    std::shared_ptr<GrpcSession> session(new GrpcSession(clientInfo, myInactivityTimerMgr));
    session->init();
    session->start();

    std::string sessionId = std::string(clientInfo.ipAddr) 
        + std::string(":") + std::string(clientInfo.port);

    std::unique_lock<std::recursive_mutex> guard(myMutex);    
    mySessionMap[sessionId] = session;
}

bool GrpcSessionMgr::hasSession(const std::string& sessionId)
{
    if(mySessionMap.find(sessionId) != mySessionMap.end())
    {
        return true;
    }
    
    return false;
}

void GrpcSessionMgr::deleteSession(const std::string& sessionId)
{
    std::cout<<"GrpcSessionMgr, deleteSession, sessionId="<<sessionId<<std::endl;

    std::unique_lock<std::recursive_mutex> guard(myMutex);

    if(hasSession(sessionId))
    {
        std::shared_ptr<GrpcSession> sessionPtr = mySessionMap[sessionId];
        mySessionMap.erase(sessionId);
        if(NULL != sessionPtr)
        {
            sessionPtr->close("Session closed normally");
            sessionPtr->join();
            sessionPtr.reset();
        }
    }
}

std::shared_ptr<GrpcSession> GrpcSessionMgr::getSession(const std::string& sessionId)
{
    return mySessionMap[sessionId];
}

