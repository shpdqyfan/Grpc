/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc service
*/

#ifndef GRPC_SERVICE_H
#define GRPC_SERVICE_H

#include <grpc/grpc.h>
#include <grpc++/grpc++.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include <string>
#include <memory>

#include "gnmi.grpc.pb.h"
#include "GrpcClientInfo.h"

class GrpcSessionMgr;
class GrpcSession;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using gnmi::Alias;
using gnmi::AliasList;
using gnmi::CapabilityRequest;
using gnmi::CapabilityResponse;
using gnmi::Decimal64;
using gnmi::GetRequest;
using gnmi::GetResponse;
using gnmi::ModelData;
using gnmi::Notification;
using gnmi::Path;
using gnmi::Poll;
using gnmi::QOSMarking;
using gnmi::ScalarArray;
using gnmi::SetRequest;
using gnmi::SetResponse;
using gnmi::SubscribeRequest;
using gnmi::SubscribeResponse;
using gnmi::Subscription;
using gnmi::SubscriptionList;
using gnmi::TypedValue;
using gnmi::Update;
using gnmi::UpdateResult;
using gnmi::Value;
using gnmi::SubscriptionList_Mode;
using gnmi::Encoding;

static const std::string ERR_MSG_FAIL_TO_GET_CLIENT_INFO = "Fail to get grpc client info";
static const std::string ERR_MSG_FAIL_TO_GET_SESSION = "Fail to get grpc session";
static const std::string ERR_MSG_FAIL_TO_SEND_SUBS_RSP = "Fail to send subscribe response";
static const std::string ERR_MSG_FAIL_TO_GET_SUBS_REQ = "Fail to get subscribe request";
static const std::string ERR_MSG_FAIL_TO_GET_SUBS_MODE = "Fail to get subscribe mode";

class GrpcService : public gnmi::gNMI::Service
{
public:
    GrpcService();
    virtual ~GrpcService();

    void registerGrpcSessionMgr(GrpcSessionMgr* ptr);

    // Capabilities allows the client to retrieve the set of capabilities that
    // is supported by the target. This allows the target to validate the
    // service version that is implemented and retrieve the set of models that
    // the target supports. The models can then be specified in subsequent RPCs
    // to restrict the set of data that is utilized.
    // Reference: gNMI Specification Section 3.2
    virtual Status Capabilities(ServerContext* context, const CapabilityRequest* request, CapabilityResponse* response);

    // Retrieve a snapshot of data from the target. A Get RPC requests that the
    // target snapshots a subset of the data tree as specified by the paths
    // included in the message and serializes this to be returned to the
    // client using the specified encoding.
    // Reference: gNMI Specification Section 3.3
    virtual Status Get(ServerContext* context, const GetRequest* request, GetResponse* response);

    // Set allows the client to modify the state of data on the target. The
    // paths to modified along with the new values that the client wishes
    // to set the value to.
    // Reference: gNMI Specification Section 3.4
    virtual Status Set(ServerContext* context, const SetRequest* request, SetResponse* response);

    // Subscribe allows a client to request the target to send it values
    // of particular paths within the data tree. These values may be streamed
    // at a particular cadence (STREAM), sent one off on a long-lived channel
    // (POLL), or sent as a one-off retrieval (ONCE).
    // Reference: gNMI Specification Section 3.5
    virtual Status Subscribe(ServerContext* context, ServerReaderWriter< SubscribeResponse, SubscribeRequest>* stream);

private:
    GrpcService(const GrpcService& gs);
    GrpcService& operator=(const GrpcService& gs);

    bool receiveSubscribeRequest(SubscribeRequest& request,
        ServerReaderWriter<SubscribeResponse, SubscribeRequest>* stream);
    bool sendSubscribeResponse(SubscribeResponse& response,
        ServerReaderWriter<SubscribeResponse, SubscribeRequest>* stream, std::shared_ptr<GrpcSession> session);
    bool cancelSubscriptions(std::shared_ptr<GrpcSession> session);
    bool getClientInfo(ServerContext* context, GrpcClientInfo& info);
    void buildErrorResponse(SubscribeResponse& subscribeRsp, grpc::StatusCode errorCode, 
        const std::string& errorMsg);
    Status buildGrpcStatus(StatusCode errorCode, const std::string& errorMsg, 
        SubscribeResponse& subscribeRsp,
        ServerReaderWriter<SubscribeResponse, SubscribeRequest>* stream);
    bool sendSubscribeRspWithoutSession(SubscribeResponse& response,
        ServerReaderWriter<SubscribeResponse, SubscribeRequest>* stream);
   
    GrpcSessionMgr* mySessionManagerPtr;
};

#endif

