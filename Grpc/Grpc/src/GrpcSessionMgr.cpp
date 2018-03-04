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
    , myDeletedSessionBuffer(new Buffer<std::string>(std::bind(&GrpcSessionMgr::deletePendingSessionCb, this, std::placeholders::_1)))
{
    std::cout<<"GrpcSessionMgr, construct"<<std::endl;
}

GrpcSessionMgr::~GrpcSessionMgr()
{ 
    std::cout<<"GrpcSessionMgr, deconstruct"<<std::endl;
    MyThread::join();
}

void GrpcSessionMgr::initialize()
{ 
    myGrpcService.registerGrpcSessionMgr(this);
}

void GrpcSessionMgr::activate()
{
    myInactivityTimerMgr->start();
    myDeletedSessionBuffer->startBuffering();
    MyThread::start();
}

void GrpcSessionMgr::stop()
{
    std::cout<<"GrpcSessionMgr, stop"<<std::endl;
    
    std::chrono::milliseconds dur(5000);
    std::chrono::time_point<std::chrono::system_clock> dt(dur);

    myRunning = false;
    
    //Shutdown the server, blocking until all rpc processing finishes. 
    //Forcefully terminate pending calls after deadline expires.
    myGrpcServer->Shutdown(dt);
    myRestartSemaphore.notify();
    
    std::unique_lock<std::recursive_mutex> guard(myMutex);
    GrpcSessionMap::iterator it;
    for(it = mySessionMap.begin();it != mySessionMap.end();++it)
    {
        if(NULL != it->second)
        {
            it->second->close("Session closed normally");
            it->second->join();
            (it->second).reset();
        }

        mySessionMap.erase(it++);
    }

    myInactivityTimerMgr->stop();
    myDeletedSessionBuffer->stopBuffering();
}

std::shared_ptr<GrpcSession> GrpcSessionMgr::requestSession(const GrpcClientInfo& clientInfo)
{
    std::unique_lock<std::recursive_mutex> guard(myMutex);

    std::string sessionId = std::string(clientInfo.ipAddr) 
        + std::string(":") + std::string(clientInfo.port);

    if(!hasSession(sessionId))
    {
        createSession(clientInfo);
    }

    return getSession(sessionId);
}

void GrpcSessionMgr::deleteSession(const std::string& sessionId)
{
    std::cout<<"GrpcSessionMgr, deleteSession, sessionId="<<sessionId<<std::endl;

    std::unique_lock<std::recursive_mutex> guard(myMutex);

    if(hasSession(sessionId))
    {
        std::shared_ptr<GrpcSession> sessionPtr = mySessionMap[sessionId];
        //mySessionMap.erase(sessionId);
        if(NULL != sessionPtr)
        {
            sessionPtr->close("Session closed normally");
            //sessionPtr->join();
            //sessionPtr.reset();
        }
    }
}

void GrpcSessionMgr::deletePendingSessionCb(const std::string& sessionId)
{
    std::cout<<"GrpcSessionMgr, deletePendingSessionCb, sessionId="<<sessionId<<std::endl;

    std::unique_lock<std::recursive_mutex> guard(myMutex);

    if(hasSession(sessionId))
    {
        std::shared_ptr<GrpcSession> sessionPtr = mySessionMap[sessionId];
        mySessionMap.erase(sessionId);
        if(NULL != sessionPtr)
        {
            sessionPtr->join();
            sessionPtr.reset();
        }
    }
    else
    {
        std::cout<<"GrpcSessionMgr, deletePendingSessionCb, couldn't find sessionId="<<sessionId<<std::endl;
    }
}

void GrpcSessionMgr::addPendingSession(const std::string& sessionId)
{
    std::cout<<"GrpcSessionMgr, addPendingSession, sessionId="<<sessionId<<std::endl;

    myDeletedSessionBuffer->pushToBuffer(sessionId);
}

void GrpcSessionMgr::run()
{
    myRunning = true;
    
    std::cout<<"GrpcSessionMgr, run ......"<<std::endl;
    
    while(myRunning)
    {
        initGRPCServer();
        if(myGrpcServer.get())
        {
            //Block until the server shuts down. 
            myGrpcServer->Wait();
        }
        
        myRestartSemaphore.wait();
    }

    std::cout<<"GrpcSessionMgr, running end"<<std::endl;
}

void GrpcSessionMgr::initGRPCServer()
{
    const std::string sAddr = myGrpcServerAddr + std::string(":") + std::to_string(myGrpcServerPort);

    //Enlists an endpoint addr (port with an optional IP address) to bind the grpc::Server object to be created to. 
    myGrpcServerBuilder.AddListeningPort(sAddr, grpc::InsecureServerCredentials());
    //Register a service. This call does not take ownership of the service. The service must exist for the lifetime 
    //of the Server instance returned by BuildAndStart(). Matches requests with any :authority 
    myGrpcServerBuilder.RegisterService(&myGrpcService);
      
    myGrpcServer.reset();
    myGrpcServer = std::move(myGrpcServerBuilder.BuildAndStart());
}

void GrpcSessionMgr::createSession(const GrpcClientInfo& clientInfo)
{
    std::shared_ptr<GrpcSession> session(new GrpcSession(clientInfo, myInactivityTimerMgr, this));
    session->init();
    session->start();

    std::string sessionId = std::string(clientInfo.ipAddr) 
        + std::string(":") + std::string(clientInfo.port);

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

std::shared_ptr<GrpcSession> GrpcSessionMgr::getSession(const std::string& sessionId)
{
    return mySessionMap[sessionId];
}

