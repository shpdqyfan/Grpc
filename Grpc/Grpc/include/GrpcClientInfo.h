/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: client info of grpc session
*/

#ifndef GRPC_CLIENT_INFO_H
#define GRPC_CLIENT_INFO_H

#include <string.h>

struct GrpcClientInfo
{
    GrpcClientInfo(const char* ip, const char* p, const char* type, const char* protocol)
    {
        strncpy(ipAddr, ip, sizeof(*ip));
        strncpy(port, p, sizeof(*p));
        strncpy(sessionType, type, sizeof(*type));
        strncpy(sessionProtocol, protocol, sizeof(*protocol));
    }
    
    GrpcClientInfo(const GrpcClientInfo& clientInfo)
    {
        strncpy(ipAddr, clientInfo.ipAddr, sizeof(clientInfo.ipAddr));
        strncpy(port, clientInfo.port, sizeof(clientInfo.port));
        strncpy(sessionType, clientInfo.sessionType, sizeof(clientInfo.sessionType));
        strncpy(sessionProtocol, clientInfo.sessionProtocol, sizeof(clientInfo.sessionProtocol));
    }

    GrpcClientInfo()
    {
        memset(ipAddr, 0, sizeof(ipAddr));
        memset(port, 0, sizeof(port));
        memset(sessionType, 0, sizeof(sessionType));
        memset(sessionProtocol, 0, sizeof(sessionProtocol));
    }

    //15 characters (12+3 separators) for IPv4; 
    //39 characters (32 + 7 separators) for IPv6
    char ipAddr[40];
    char port[10];
    char sessionType[8];
    char sessionProtocol[8];
};

#endif

