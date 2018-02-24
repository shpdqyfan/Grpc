/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc service
*/

#include <iostream>
#include <unistd.h>

#include "GrpcService.h"
#include "GrpcSession.h"
#include "GrpcSessionMgr.h"

GrpcService::GrpcService()
{
    std::cout<<"GrpcService, construct"<<std::endl;
}

GrpcService::~GrpcService()
{ 
    std::cout<<"GrpcService, deconstruct"<<std::endl;
}

void GrpcService::registerGrpcSessionMgr(GrpcSessionMgr* ptr)
{
    mySessionManagerPtr = ptr;
}

::grpc::Status GrpcService::Capabilities(::grpc::ServerContext* context, 
    const ::gnmi::CapabilityRequest* request, ::gnmi::CapabilityResponse* response)
{
    //TODO:
    return ::grpc::Status::OK;
}

::grpc::Status GrpcService::Get(::grpc::ServerContext* context, 
    const ::gnmi::GetRequest* request, ::gnmi::GetResponse* response)
{
    //TODO:
    return ::grpc::Status::OK;
}

::grpc::Status GrpcService::Set(::grpc::ServerContext* context, 
    const ::gnmi::SetRequest* request, ::gnmi::SetResponse* response)
{
    //TODO:
    return ::grpc::Status::OK;
}

::grpc::Status GrpcService::Subscribe(::grpc::ServerContext* context, 
    ::grpc::ServerReaderWriter<::gnmi::SubscribeResponse, ::gnmi::SubscribeRequest>* stream)  
{
    ::gnmi::SubscribeResponse subRsp;
    ::grpc::StatusCode staCode = ::grpc::OK;
    GrpcClientInfo cInfo;
    std::string errMsg("");

    if(!getClientInfo(context, cInfo))
    {
        return buildGrpcStatus(::grpc::PERMISSION_DENIED, ERR_MSG_FAIL_TO_GET_CLIENT_INFO, subRsp, stream);
    }

    std::shared_ptr<GrpcSession> session = mySessionManagerPtr->requestSession(cInfo);
    if(NULL == session)
    {
        return buildGrpcStatus(::grpc::UNAUTHENTICATED, ERR_MSG_FAIL_TO_GET_SESSION, subRsp, stream);
    }

    while(GrpcSession::INIT == session->getStatus())
    {
        sleep(1);
    }

    //get request
    ::gnmi::SubscribeRequest req;
    if(receiveSubscribeRequest(req, stream) && req.has_subscribe())
    {
        //session->setTimer(GNMI_DEFAULT_SESSION_INACTIVE_TIMEOUT); 
        //session->restartTimer();

        switch(req.subscribe().mode())
        {
            case ::gnmi::SubscriptionList_Mode_STREAM:
                //TODO:
                break;
            case ::gnmi::SubscriptionList_Mode_ONCE:
            case ::gnmi::SubscriptionList_Mode_POLL:
            default:
                return buildGrpcStatus(::grpc::INVALID_ARGUMENT, ERR_MSG_FAIL_TO_GET_SUBS_MODE, subRsp, stream);
        }
    }
    else
    {
        return buildGrpcStatus(::grpc::INVALID_ARGUMENT, ERR_MSG_FAIL_TO_GET_SUBS_REQ, subRsp, stream);
    }

    req.clear_subscribe();     

    //send response
    if(GrpcSession::ENABLED == session->getStatus())
    {
        subRsp.clear_update();
        sendSubscribeResponse(subRsp, stream, session);
    }
    //session->setTimer(GNMI_DEFAULT_SESSION_INACTIVE_TIMEOUT); 
    //session->restartTimer();
       
    return Status(staCode, errMsg); 
}

bool GrpcService::receiveSubscribeRequest(::gnmi::SubscribeRequest& request, 
    ::grpc::ServerReaderWriter<::gnmi::SubscribeResponse, ::gnmi::SubscribeRequest>* stream)
{
    return stream->Read(&request);
}

bool GrpcService::sendSubscribeResponse(::gnmi::SubscribeResponse& response, 
    ::grpc::ServerReaderWriter<::gnmi::SubscribeResponse, ::gnmi::SubscribeRequest>* stream, 
    std::shared_ptr<GrpcSession> session)
{
    return stream->Write(response);
}

bool GrpcService::cancelSubscriptions(std::shared_ptr<GrpcSession> session)
{
    return true;
}

bool GrpcService::getClientInfo(::grpc::ServerContext* context, GrpcClientInfo& info)
{
    std::string peer(context->peer());
    if("unknow" == peer)
    {
        return false;
    }

    std::cout<<"GrpcService, getClientInfo, peer="<<peer<<std::endl;
    //peer str format is "ipv4:addrstr:portstr" or "ipv6:addrstr:portstr"
    int first = peer.find(":");
    int last = peer.rfind(":");
    std::string ip = peer.substr(first+1, last-first-1);
    std::string port = peer.substr(last+1);

    strncpy(info.ipAddr, ip.c_str(), sizeof(info.ipAddr));
    strncpy(info.port, port.c_str(), sizeof(info.port));
    strncpy(info.sessionType, "gnmi", sizeof(info.sessionType));
    strncpy(info.sessionProtocol, "http", sizeof(info.sessionProtocol));

    return true;
}

void GrpcService::buildErrorResponse(::gnmi::SubscribeResponse& subscribeRsp, 
    ::grpc::StatusCode errorCode, const std::string& errorMsg)
{
    ::gnmi::Error* gnmiError = new gnmi::Error();

    gnmiError->set_code(errorCode);
    gnmiError->set_allocated_message(new std::string(errorMsg));
    subscribeRsp.set_allocated_error(gnmiError);
}

::grpc::Status GrpcService::buildGrpcStatus(::grpc::StatusCode errorCode, const std::string& errorMsg,
    ::gnmi::SubscribeResponse& subscribeRsp,
    ::grpc::ServerReaderWriter<::gnmi::SubscribeResponse, ::gnmi::SubscribeRequest>* stream)
{
    ::grpc::StatusCode staCode = errorCode;
    std::string errMsg = errorMsg;
        
    buildErrorResponse(subscribeRsp, staCode, errMsg);
    if(!sendSubscribeRspWithoutSession(subscribeRsp, stream))
    {
        staCode = ::grpc::CANCELLED;
        errMsg = ERR_MSG_FAIL_TO_SEND_SUBS_RSP;
    }
        
    return Status(staCode, errMsg);    
}

bool GrpcService::sendSubscribeRspWithoutSession(::gnmi::SubscribeResponse& response, 
    ::grpc::ServerReaderWriter<::gnmi::SubscribeResponse, ::gnmi::SubscribeRequest>* stream)
{
    return stream->Write(response);
}

