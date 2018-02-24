/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc
*/

#include <iostream>
#include <memory>

#include "GrpcSessionMgr.h"

static std::unique_ptr<GrpcSessionMgr> myGrpcSessionMgrPtr(new GrpcSessionMgr("127.0.0.1", 50001));

int main()
{
    std::cout<<"Hello, Grpc"<<std::endl;

    return 0;
}

