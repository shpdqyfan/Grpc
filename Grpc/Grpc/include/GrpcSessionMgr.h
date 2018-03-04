/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc session manager
               Handles all sessions. It waits for new connection
               and create grpc session in a new thread. In the 
               end it is notified when a session ends to delete
               the grpc session.
*/

#ifndef GRPC_SESSION_MGR_H
#define GRPC_SESSION_MGR_H

#include <string>
#include <memory>
#include <map>
#include <mutex>

#include "Thread/Thread.h"
#include "Semaphore/Semaphore.h"
#include "GrpcService.h"

class GrpcSession;
class GrpcClientInfo;
class EasyTimer;

class GrpcSessionMgr : public MyThread
{
    typedef std::map<std::string, std::shared_ptr<GrpcSession>> GrpcSessionMap;

public:
    GrpcSessionMgr(const std::string& addr, int port);
    ~GrpcSessionMgr();

    void initialize();
    void activate();
    void stop();
    std::shared_ptr<GrpcSession> requestSession(const GrpcClientInfo& clientInfo);
    void deleteSession(const std::string& sessionId);
    
protected:
    virtual void run();

private:
    void initGRPCServer();
    void createSession(const GrpcClientInfo& clientInfo);
    bool hasSession(const std::string& sessionId);
    std::shared_ptr<GrpcSession> getSession(const std::string& sessionId);
    
    std::string myGrpcServerAddr;
    int myGrpcServerPort;
    //A builder class for the creation and startup of grpc::Server instances
    ServerBuilder myGrpcServerBuilder;    
    //Represents a gRPC server. Use a grpc::ServerBuilder to create, configure, and start Server instances.
    std::unique_ptr<Server> myGrpcServer;
    //Desriptor of an RPC service and its various RPC methods. 
    GrpcService myGrpcService;
    bool myRunning;
    GrpcSessionMap mySessionMap;
    std::recursive_mutex myMutex;
    Semaphore myRestartSemaphore;
    std::shared_ptr<EasyTimer> myInactivityTimerMgr;
};

#endif

